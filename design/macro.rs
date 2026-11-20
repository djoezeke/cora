// ==================================================================
// 1. Declarative Macro Example (macro_rules!)
// ==================================================================
trait Describe {
    fn describe(&self) -> &'static str;
}

macro_rules! impl_describe {
    // Match one or more type identifiers separated by commas
    ( $( $t:ty ),+ ) => {
        $(
            impl Describe for $t {
                fn describe(&self) -> &'static str {
                    // stringify! converts the type token into a string literal
                    stringify!($t)
                }
            }
        )+
    };
}

struct User;
struct Invoice;
struct Order;

// Generate implementations for all three structs at once
impl_describe!(User, Invoice, Order);

fn main() {
    let u = User;
    let i = Invoice;
    println!("Type 1: {}, Type 2: {}", u.describe(), i.describe());
}

// ==================================================================
// 2. Procedural Macro Example (#[derive(Describe)])
// ==================================================================

use proc_macro::TokenStream;
use quote::quote;
use syn::{parse_macro_input, DeriveInput};

#[proc_macro_derive(TypeName)]
pub fn derive_type_name(input: TokenStream) -> TokenStream {
    let ast = parse_macro_input!(input as DeriveInput);
    let name = &ast.ident;

    quote! {
        impl #name {
            pub fn type_name() -> &'static str {
                stringify!(#name)
            }
        }
    }
    .into()
}

// Usage example:
use my_macros::TypeName;

#[derive(TypeName)]
struct Customer {
    id: u64,
}

// Calls the generated inherent method:
assert_eq!(Customer::type_name(), "Customer");

// ==================================================================
// 3. Procedural Macro Example (#[attr])
// ==================================================================
use proc_macro::TokenStream;
use quote::quote;
use syn::{parse_macro_input, ItemFn, LitStr};

#[proc_macro_attribute]
pub fn tag_fn(attr: TokenStream, item: TokenStream) -> TokenStream {
    // `attr` holds what is inside #[tag_fn("...")]
    let tag = parse_macro_input!(attr as LitStr);
    // `item` holds the function below the attribute
    let input_fn = parse_macro_input!(item as ItemFn);

    let vis = &input_fn.vis;
    let sig = &input_fn.sig;
    let block = &input_fn.block;

    // Replaces the original function with a modified version
    quote! {
        #vis #sig {
            println!("[TAG: {}]", #tag);
            #block
        }
    }
    .into()
}

// Usage example:
use my_macros::tag_fn;

#[tag_fn("AUTH")]
fn login(user: &str) {
    println!("Logging in {user}");
}

// Running login("ada") prints:
// [TAG: AUTH]
// Logging in ada


// ==================================================================
// 4. Procedural Macro Example (func!())
// ==================================================================
use proc_macro::TokenStream;
use quote::quote;
use syn::parse::{Parse, ParseStream};
use syn::{parse_macro_input, Expr, Token};

// Custom parser for `key => value` syntax
struct KeyValue {
    key: Expr,
    _arrow: Token![=>],
    value: Expr,
}

impl Parse for KeyValue {
    fn parse(input: ParseStream) -> syn::Result<Self> {
        Ok(KeyValue {
            key: input.parse()?,
            _arrow: input.parse()?,
            value: input.parse()?,
        })
    }
}

#[proc_macro]
pub fn pair(input: TokenStream) -> TokenStream {
    let KeyValue { key, value, .. } = parse_macro_input!(input as KeyValue);

    // Expands directly into a Rust tuple expression
    quote! {
        (#key, #value)
    }
    .into()
}

// Usage example:
use my_macros::pair;

fn main() {
    // Custom `=>` syntax parsed at compile time into ("port", 8080)
    let config = pair!("port" => 8080);
    assert_eq!(config, ("port", 8080));
}

