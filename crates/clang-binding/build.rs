use regex::Regex;
use std::env;
use std::fmt::format;
use std::path::PathBuf;

fn main() {
    // println!("cargo:rustc-link-lib=static=");
    println!("cargo:rustc-link-lib=stdc++");

    let tmp = PathBuf::from(".").canonicalize().unwrap();
    let cur = tmp.to_str().unwrap();

    let is_wasm = env::var("CARGO_CFG_TARGET_ARCH").as_deref() == Ok("wasm32");

    let base_dir = if is_wasm {
        format!("{cur}/llvm-project/build/wasm/install")
    } else {
        format!("{cur}/llvm-project/build/install")
    };

    println!("cargo:rustc-link-search=native={base_dir}/lib");

    let mut clang_proc = std::process::Command::new("clang");
    clang_proc
        .arg("-c")
        .arg("-o")
        .arg(format!("{base_dir}/lib/wrapper.o"))
        .arg("./src/wrapper.cpp")
        .arg("-std=c++17")
        .arg(format!("-I{base_dir}/include/"));

    if is_wasm {
        clang_proc.arg("-target").arg("wasm32-wasip1");
        clang_proc.arg("-I./wasi-sysroot-24.0/include/wasm32-wasip1/c++/v1/");
        clang_proc.arg("-I./wasi-sysroot-24.0/include/wasm32-wasip1/");
        clang_proc.arg("-D__WASM__");
    }

    let clang_proc_res = clang_proc.output().expect("could not spawn `clang`");
    if !clang_proc_res.status.success() {
        eprintln!(
            "clang failed: {}",
            String::from_utf8_lossy(&clang_proc_res.stderr)
        );
        std::process::exit(1);
    }

    if !std::process::Command::new("ar")
        .arg("rcs")
        .arg(format!("{base_dir}/lib/libwrapper.a"))
        .arg(format!("{base_dir}/lib/wrapper.o"))
        .output()
        .expect("could not spawn `ar`")
        .status
        .success()
    {
        panic!("could not emit library file");
    }

    for entry in std::fs::read_dir(format!("{base_dir}/lib/")).unwrap() {
        let entry = entry.unwrap();
        let path = entry.path();
        if path.is_file() {
            || -> Option<()> {
                let path = path.file_name()?.to_str()?;
                let lib_name = Regex::new(r"lib(.*)\.a")
                    .ok()?
                    .captures(path)?
                    .get(1)?
                    .as_str();
                println!("cargo:rustc-link-lib={}", lib_name);
                Some(())
            }();
        }
    }

    println!("cargo::rerun-if-changed=build.rs");
    println!("cargo::rerun-if-changed=src/wrapper.cpp");
    println!("cargo::rerun-if-changed=src/wrapper.hpp");
}
