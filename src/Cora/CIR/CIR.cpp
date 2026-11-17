#include <Cora/CIR/CIR.h>

#include <Cora/Core/SHA256.h>

#include <charconv>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace cora::cir
{
    namespace
    {
        constexpr char HexDigits[] = "0123456789abcdef";

        std::string encodeHex(std::string_view text)
        {
            std::string result;
            result.reserve(text.size() * 2);
            for (const unsigned char character : text)
            {
                result.push_back(HexDigits[character >> 4]);
                result.push_back(HexDigits[character & 0x0f]);
            }
            return result;
        }

        int hexDigit(char character) noexcept
        {
            if (character >= '0' && character <= '9')
                return character - '0';
            if (character >= 'a' && character <= 'f')
                return character - 'a' + 10;
            if (character >= 'A' && character <= 'F')
                return character - 'A' + 10;
            return -1;
        }

        std::optional<std::string> decodeHex(std::string_view text)
        {
            if (text.size() % 2 != 0)
                return std::nullopt;
            std::string result;
            result.reserve(text.size() / 2);
            for (std::size_t index = 0; index < text.size(); index += 2)
            {
                const int high = hexDigit(text[index]);
                const int low = hexDigit(text[index + 1]);
                if (high < 0 || low < 0)
                    return std::nullopt;
                result.push_back(static_cast<char>((high << 4) | low));
            }
            return result;
        }

        std::optional<std::string> field(std::string_view text, std::string_view name)
        {
            const std::string prefix = std::string(name) + "=";
            std::size_t begin = 0;
            while (begin < text.size())
            {
                const std::size_t end = text.find('\n', begin);
                const std::string_view line = text.substr(
                    begin, end == std::string_view::npos ? text.size() - begin
                                                         : end - begin);
                if (line.size() >= prefix.size() &&
                    line.compare(0, prefix.size(), prefix) == 0)
                    return std::string(line.substr(prefix.size()));
                if (end == std::string_view::npos)
                    break;
                begin = end + 1;
            }
            return std::nullopt;
        }

        std::optional<std::string> hexField(std::string_view text, std::string_view name)
        {
            const auto value = field(text, name);
            if (!value)
                return std::nullopt;
            return decodeHex(*value);
        }

        std::optional<std::size_t> numberField(std::string_view text,
                                               std::string_view name)
        {
            const auto value = field(text, name);
            if (!value)
                return std::nullopt;
            std::size_t result = 0;
            const auto parsed =
                std::from_chars(value->data(), value->data() + value->size(), result);
            if (parsed.ec != std::errc{} || parsed.ptr != value->data() + value->size())
                return std::nullopt;
            return result;
        }

        void appendHexField(std::ostringstream &out, std::string_view name,
                            std::string_view value)
        {
            out << name << '=' << encodeHex(value) << '\n';
        }
    } // namespace

    const char *tagName(Tag tag) noexcept
    {
        switch (tag)
        {
        case Tag::Invalid: return "Invalid";
        case Tag::StructDecl: return "StructDecl";
        case Tag::FnDecl: return "FnDecl";
        case Tag::LetDecl: return "LetDecl";
        case Tag::TypeAlias: return "TypeAlias";
        case Tag::Import: return "Import";
        case Tag::Block: return "Block";
        case Tag::ExprStmt: return "ExprStmt";
        case Tag::If: return "If";
        case Tag::While: return "While";
        case Tag::ForC: return "ForC";
        case Tag::ForRange: return "ForRange";
        case Tag::Return: return "Return";
        case Tag::Break: return "Break";
        case Tag::Continue: return "Continue";
        case Tag::Defer: return "Defer";
        case Tag::Identifier: return "Identifier";
        case Tag::IntLit: return "IntLit";
        case Tag::FloatLit: return "FloatLit";
        case Tag::StringLit: return "StringLit";
        case Tag::CharLit: return "CharLit";
        case Tag::BoolLit: return "BoolLit";
        case Tag::NullLit: return "NullLit";
        case Tag::Unary: return "Unary";
        case Tag::Binary: return "Binary";
        case Tag::Assign: return "Assign";
        case Tag::Call: return "Call";
        case Tag::FieldAccess: return "FieldAccess";
        case Tag::Index: return "Index";
        case Tag::ArrayInit: return "ArrayInit";
        case Tag::StructInit: return "StructInit";
        case Tag::Paren: return "Paren";
        case Tag::Malformed: return "Malformed";
        }
        return "?";
    }

    void CIR::dump(std::ostream &out) const
    {
        out << "CIR bodies=" << bodies.size() << " strings=" << strings.size()
            << " types=" << types.size() << " errors=" << errors.size() << '\n';

        for (std::size_t body_index = 0; body_index < bodies.size(); ++body_index)
        {
            const Body &body = bodies[body_index];
            out << "body[" << body_index << "] " << body.name;
            if (body.is_extern)
                out << " extern";
            out << " params=" << body.params.size() << '\n';

            for (std::size_t index = 1; index < body.insts.size(); ++index)
            {
                const Inst &instruction = body.insts[index];
                out << "  [" << index << "] " << tagName(instruction.tag);

                if (instruction.name != TNull)
                    out << " name=" << string(instruction.name);
                if (instruction.op != Token::Tag::T_EOF)
                    out << " op=" << static_cast<int>(instruction.op);
                if (instruction.tag == Tag::IntLit ||
                    instruction.tag == Tag::CharLit ||
                    instruction.tag == Tag::BoolLit)
                    out << " value=" << instruction.int_value;
                if (instruction.tag == Tag::BoolLit)
                    out << " bool=" << (instruction.bool_value ? "true" : "false");
                if (instruction.tag == Tag::FloatLit)
                    out << " value=" << instruction.float_value;
                if (instruction.tag == Tag::StringLit)
                    out << " text=\"" << string(instruction.name) << '"';
                if (instruction.type_ref != TNull)
                    out << " type=" << instruction.type_ref;
                if (instruction.a != INull) out << " a=" << instruction.a;
                if (instruction.b != INull) out << " b=" << instruction.b;
                if (instruction.c != INull) out << " c=" << instruction.c;
                if (instruction.d != INull) out << " d=" << instruction.d;
                if (instruction.extra != ENull)
                    out << " extra=" << instruction.extra
                        << '+' << instruction.extra_count;
                out << '\n';
            }
        }

        for (const std::string &error : errors)
            out << "error: " << error << '\n';
    }

    std::string CIRInput::canonical() const
    {
        std::ostringstream output;
        output << frontendVersion << '\n'
               << sourceFileId << '\n'
               << moduleId << '\n'
               << sourceDigest.hex() << '\n'
               << frontendOptions << '\n';
        for (const auto &digest : generatedInputDigests)
            output << digest.hex() << '\n';
        return output.str();
    }

    core::Digest CIRInput::key() const
    {
        return crypto::SHA256::hash("cora-cir-key-v3\n" + canonical());
    }

    std::vector<std::uint8_t> CIRArtifact::serialize() const
    {
        std::ostringstream out;
        out << "CORA-CIR-V3\n"
            << "key=" << key.hex() << '\n'
            << "source-file=" << encodeHex(input.sourceFileId) << '\n'
            << "module=" << encodeHex(input.moduleId) << '\n'
            << "source-digest=" << input.sourceDigest.hex() << '\n'
            << "interface-digest=" << publicInterfaceDigest.hex() << '\n'
            << "frontend-version=" << encodeHex(input.frontendVersion) << '\n'
            << "frontend-options=" << encodeHex(input.frontendOptions) << '\n'
            << "succeeded=" << (succeeded ? 1 : 0) << '\n';
        appendHexField(out, "source", source);
        appendHexField(out, "tokens", tokensDump);
        appendHexField(out, "ast", astDump);
        appendHexField(out, "cir", cirDump);
        appendHexField(out, "air", airDump);
        appendHexField(out, "c", cSource);
        out << "diagnostics=" << diagnostics.size() << '\n';
        for (const std::string &diagnostic : diagnostics)
            appendHexField(out, "diag", diagnostic);

        const std::string text = out.str();
        return {text.begin(), text.end()};
    }

    std::optional<CIRArtifact> CIRArtifact::deserialize(
        const std::vector<std::uint8_t> &bytes, std::string *error)
    {
        const std::string text(bytes.begin(), bytes.end());
        if (text.rfind("CORA-CIR-V3\n", 0) != 0)
        {
            if (error)
                *error = "invalid CIR artifact magic or version";
            return std::nullopt;
        }

        CIRArtifact result;
        const auto key = field(text, "key");
        const auto sourceFile = hexField(text, "source-file");
        const auto module = hexField(text, "module");
        const auto sourceDigest = field(text, "source-digest");
        const auto interfaceDigest = field(text, "interface-digest");
        const auto frontendVersion = hexField(text, "frontend-version");
        const auto frontendOptions = hexField(text, "frontend-options");
        const auto succeeded = numberField(text, "succeeded");
        if (!key || !sourceFile || !module || !sourceDigest || !interfaceDigest ||
            !frontendVersion || !frontendOptions || !succeeded)
        {
            if (error)
                *error = "invalid CIR artifact metadata";
            return std::nullopt;
        }

        const auto parsedKey = core::Digest::fromHex(*key);
        const auto parsedSource = core::Digest::fromHex(*sourceDigest);
        const auto parsedInterface = core::Digest::fromHex(*interfaceDigest);
        if (!parsedKey || !parsedSource || !parsedInterface)
        {
            if (error)
                *error = "invalid CIR artifact digest";
            return std::nullopt;
        }

        result.key = *parsedKey;
        result.input.sourceFileId = *sourceFile;
        result.input.moduleId = *module;
        result.input.sourceDigest = *parsedSource;
        result.input.frontendVersion = *frontendVersion;
        result.input.frontendOptions = *frontendOptions;
        result.publicInterfaceDigest = *parsedInterface;
        result.succeeded = *succeeded != 0;

        const auto source = hexField(text, "source");
        const auto tokens = hexField(text, "tokens");
        const auto ast = hexField(text, "ast");
        const auto cirDump = hexField(text, "cir");
        const auto air = hexField(text, "air");
        const auto c = hexField(text, "c");
        if (!source || !tokens || !ast || !cirDump || !air || !c)
        {
            if (error)
                *error = "invalid CIR artifact payload";
            return std::nullopt;
        }
        result.source = *source;
        result.tokensDump = *tokens;
        result.astDump = *ast;
        result.cirDump = *cirDump;
        result.airDump = *air;
        result.cSource = *c;

        const auto diagnosticCount = numberField(text, "diagnostics");
        if (!diagnosticCount)
        {
            if (error)
                *error = "invalid CIR diagnostic count";
            return std::nullopt;
        }
        std::size_t begin = 0;
        while (result.diagnostics.size() < *diagnosticCount && begin < text.size())
        {
            const std::size_t end = text.find('\n', begin);
            const std::string line = text.substr(
                begin, end == std::string::npos ? text.size() - begin : end - begin);
            if (line.rfind("diag=", 0) == 0)
            {
                const auto decoded = decodeHex(std::string_view(line).substr(5));
                if (!decoded)
                {
                    if (error)
                        *error = "invalid CIR diagnostic payload";
                    return std::nullopt;
                }
                result.diagnostics.push_back(*decoded);
            }
            if (end == std::string::npos)
                break;
            begin = end + 1;
        }
        if (result.diagnostics.size() != *diagnosticCount)
        {
            if (error)
                *error = "truncated CIR diagnostic payload";
            return std::nullopt;
        }
        return result;
    }

} // namespace cora::cir
