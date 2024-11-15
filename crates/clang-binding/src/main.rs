use std::path::PathBuf;
use clang_binding::format;

fn main() {
    println!("{:?}", format(&PathBuf::from("./build.rs"), "int main() {    return 0; }"));
}