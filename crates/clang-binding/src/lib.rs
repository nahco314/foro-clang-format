use crate::bindings::parse_result;
use anyhow::{Context, Result};
use std::path::PathBuf;

pub mod bindings;
pub mod is_ignore;

#[derive(Debug, Clone)]
pub enum FormatResult {
    Success { formatted_content: String },
    Error { error: String },
    Ignored,
}

pub fn format(target_path: &PathBuf, target_content: &str) -> Result<FormatResult> {
    if is_ignore::is_ignored(target_path) {
        return Ok(FormatResult::Ignored);
    }

    let file_name = target_path
        .to_str()
        .context("Failed to convert path to string")?;
    let code = target_content;

    let file_name_c =
        std::ffi::CString::new(file_name).context("Failed to convert file name to CString")?;
    let code_c = std::ffi::CString::new(code).context("Failed to convert code to CString")?;

    let file_name_ptr = file_name_c.as_ptr() as *mut std::os::raw::c_char;
    let code_ptr = code_c.as_ptr() as *mut std::os::raw::c_char;

    let res = unsafe {
        let raw_res = bindings::wrapper_main(file_name_ptr, code_ptr);
        parse_result(raw_res)
    };

    Ok(match res {
        Ok(s) => FormatResult::Success {
            formatted_content: s,
        },
        Err(e) => FormatResult::Error { error: e },
    })
}
