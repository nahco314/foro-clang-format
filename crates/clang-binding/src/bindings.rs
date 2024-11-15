extern "C" {
    pub fn wrapper_main(
        file_name: *mut std::os::raw::c_char,
        code: *mut std::os::raw::c_char,
    ) -> *mut std::os::raw::c_char;
}

// result string is following format:
// - if format success, first byte is '0' and the rest is the formatted code
// - if format failed, first byte is '1' and the rest is the error message

pub fn parse_result(res: *const std::os::raw::c_char) -> Result<String, String> {
    unsafe {
        let res_str = std::ffi::CStr::from_ptr(res).to_str().unwrap();
        let res_str = res_str.trim_start();
        match res_str.chars().next() {
            Some('0') => Ok(res_str[1..].to_string()),
            Some('1') => Err(res_str[1..].to_string()),
            _ => Err("Unknown error".to_string()),
        }
    }
}
