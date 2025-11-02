// Mathematical operations example
fn multiply(a: i32, b: i32) -> i32 {
    return a * b;
}

fn divide(a: i32, b: i32) -> i32 {
    return a / b;
}

fn main() -> i32 {
    let x: i32 = 100;
    let y: i32 = 5;
    let product: i32 = multiply(x, y);
    let quotient: i32 = divide(x, y);
    return product + quotient;
}

