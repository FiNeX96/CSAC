use std::env;
use std::fs;

fn main() {
    // Get the value of MAX_N from the environment variable
    let max_n = env::var("MAX_N").unwrap_or_else(|_| "60".to_string());
    let max_n: usize = max_n.parse().expect("MAX_N must be a valid integer");

    // Generate Rust code to define MAX_N as a constant
    let code = format!(
        r#"pub const MAX_N: usize = {};"#,
        max_n
    );

    // Write the generated code to a file
    fs::write("src/max_n.rs", code).expect("Failed to write max_n.rs");

    println!("cargo:rerun-if-env-changed=MAX_N");
}
