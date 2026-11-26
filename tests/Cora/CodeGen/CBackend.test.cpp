#include <Cora/AIR/AIR.h>
#include <Cora/CIR/CIR.h>
#include <Cora/CodeGen/CBackend.h>

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <string>

namespace
{
    cora::cir::TIndex add_named_type(cora::cir::CIR &module,
                                     const std::string &name)
    {
        cora::cir::TypeRefInfo type;
        type.kind = cora::cir::TypeRefKind::Named;
        type.name = module.internString(name);
        return module.addType(std::move(type));
    }

    cora::cir::Body &add_main(cora::cir::CIR &module,
                              std::int64_t return_value = 0)
    {
        const auto index = module.addBody("main");
        cora::cir::Body &body = module.bodies[index];
        body.is_function = true;
        body.return_type = add_named_type(module, "u32");

        cora::cir::Inst literal;
        literal.tag = cora::cir::Tag::IntLit;
        literal.int_value = return_value;
        const auto literal_index = body.add(literal);

        cora::cir::Inst result;
        result.tag = cora::cir::Tag::Return;
        result.a = literal_index;
        body.roots.push_back(body.add(result));
        return body;
    }

    std::string generate(const cora::cir::CIR &module)
    {
        const auto air = cora::air::SemanticAnalyzer{}.analyze(module);
        REQUIRE(air.verified);
        return cora::codegen::CBackend{}.generate(module, air);
    }
} // namespace

TEST_CASE("C backend emits a main function from verified AIR", "[codegen][c]")
{
    cora::cir::CIR module;
    add_main(module, 42);

    const std::string source = generate(module);
    REQUIRE(source.find("#include <stdint.h>") != std::string::npos);
    REQUIRE(source.find("int main(void)") != std::string::npos);
    REQUIRE(source.find("return 42;") != std::string::npos);
}

TEST_CASE("C backend emits struct fields using their C types",
          "[codegen][types]")
{
    cora::cir::CIR module;
    const auto point_index = module.addBody("Point");
    cora::cir::Body &point = module.bodies[point_index];
    point.fields.push_back(
        {module.internString("x"), add_named_type(module, "i32"), {}});
    point.fields.push_back(
        {module.internString("y"), add_named_type(module, "f64"), {}});
    add_main(module);

    const std::string source = generate(module);
    REQUIRE(source.find("typedef struct Point") != std::string::npos);
    REQUIRE(source.find("int32_t x;") != std::string::npos);
    REQUIRE(source.find("double y;") != std::string::npos);
}

TEST_CASE("C backend emits aggregate initialization and field access",
          "[codegen][aggregates]")
{
    cora::cir::CIR module;
    const auto point_index = module.addBody("Point");
    cora::cir::Body &point = module.bodies[point_index];
    const auto integer_type = add_named_type(module, "i32");
    point.fields.push_back(
        {module.internString("x"), integer_type, {}});
    point.fields.push_back(
        {module.internString("y"), integer_type, {}});

    const auto main_index = module.addBody("main");
    cora::cir::Body &main = module.bodies[main_index];
    main.is_function = true;
    main.return_type = integer_type;
    const auto point_type = add_named_type(module, "Point");

    cora::cir::Inst x_value;
    x_value.tag = cora::cir::Tag::IntLit;
    x_value.int_value = 1;
    const auto x_value_index = main.add(x_value);

    cora::cir::Inst y_value;
    y_value.tag = cora::cir::Tag::IntLit;
    y_value.int_value = 2;
    const auto y_value_index = main.add(y_value);

    const auto x_name = module.internString("x");
    const auto y_name = module.internString("y");
    const auto point_name = module.internString("Point");

    cora::cir::Inst aggregate;
    aggregate.tag = cora::cir::Tag::StructInit;
    aggregate.name = point_name;
    aggregate.extra = main.addList(
        {x_name, x_value_index, y_name, y_value_index});
    aggregate.extra_count = 4;
    aggregate.type_ref = point_type;
    const auto aggregate_index = main.add(aggregate);

    cora::cir::Inst declaration;
    declaration.tag = cora::cir::Tag::LetDecl;
    declaration.name = module.internString("point");
    declaration.type_ref = point_type;
    declaration.b = aggregate_index;
    main.roots.push_back(main.add(declaration));

    cora::cir::Inst identifier;
    identifier.tag = cora::cir::Tag::Identifier;
    identifier.name = module.internString("point");
    identifier.type_ref = point_type;
    const auto identifier_index = main.add(identifier);

    cora::cir::Inst field;
    field.tag = cora::cir::Tag::FieldAccess;
    field.a = identifier_index;
    field.name = x_name;
    const auto field_index = main.add(field);

    cora::cir::Inst result;
    result.tag = cora::cir::Tag::Return;
    result.a = field_index;
    main.roots.push_back(main.add(result));

    const std::string source = generate(module);
    REQUIRE(source.find(".x = 1") != std::string::npos);
    REQUIRE(source.find(".y = 2") != std::string::npos);
    REQUIRE(source.find("point.x") != std::string::npos);
}

TEST_CASE("C backend erases generic field types to portable C types",
          "[codegen][generics]")
{
    cora::cir::CIR module;
    const auto box_index = module.addBody("Box");
    cora::cir::Body &box = module.bodies[box_index];
    box.fields.push_back(
        {module.internString("value"), add_named_type(module, "T"), {}});

    const auto alias_index = module.addBody("IntBox");
    cora::cir::Body &alias = module.bodies[alias_index];
    alias.is_type_alias = true;
    alias.alias_type = add_named_type(module, "Box");
    add_main(module);

    const std::string source = generate(module);
    REQUIRE(source.find("typedef struct Box") != std::string::npos);
    REQUIRE(source.find("int64_t value;") != std::string::npos);
    REQUIRE(source.find("typedef Box IntBox;") != std::string::npos);
    REQUIRE(source.find("<T>") == std::string::npos);
}
