use std::path::PathBuf;
use clang_binding::format;

fn main() {
    let file_name =
        "/home/nahco314/RustroverProjects/foro-clang-format/crates/clang-binding/src/wrapper.cpp";
    let code = "int    main() { return     }";

    println!("{:?}", format(&PathBuf::from(file_name), code));
}
