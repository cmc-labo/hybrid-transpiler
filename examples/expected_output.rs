// Expected Rust output for simple_class.cpp

pub struct Point {
    x: i32,
    y: i32,
}

impl Point {
    pub fn new(x: i32, y: i32) -> Self {
        Self { x, y }
    }

    pub fn get_x(&self) -> i32 {
        self.x
    }

    pub fn get_y(&self) -> i32 {
        self.y
    }

    pub fn set_x(&mut self, new_x: i32) {
        self.x = new_x;
    }

    pub fn set_y(&mut self, new_y: i32) {
        self.y = new_y;
    }

    pub fn distance_squared(&self) -> i32 {
        self.x * self.x + self.y * self.y
    }

    pub fn translate(&mut self, dx: i32, dy: i32) {
        self.x += dx;
        self.y += dy;
    }
}

pub struct Rectangle {
    top_left: Point,
    bottom_right: Point,
}

impl Rectangle {
    pub fn new(tl: Point, br: Point) -> Self {
        Self {
            top_left: tl,
            bottom_right: br,
        }
    }

    pub fn width(&self) -> i32 {
        self.bottom_right.get_x() - self.top_left.get_x()
    }

    pub fn height(&self) -> i32 {
        self.bottom_right.get_y() - self.top_left.get_y()
    }

    pub fn area(&self) -> i32 {
        self.width() * self.height()
    }
}

pub struct Resource {
    data: Vec<i32>,
    size: usize,
}

impl Resource {
    pub fn new(n: usize) -> Self {
        Self {
            data: vec![0; n],
            size: n,
        }
    }

    pub fn get(&self, index: usize) -> i32 {
        self.data[index]
    }

    pub fn get_mut(&mut self, index: usize) -> &mut i32 {
        &mut self.data[index]
    }

    pub fn get_size(&self) -> usize {
        self.size
    }
}

use std::rc::Rc;
use std::cell::RefCell;

pub struct SharedData {
    message: Rc<RefCell<String>>,
}

impl SharedData {
    pub fn new(msg: &str) -> Self {
        Self {
            message: Rc::new(RefCell::new(msg.to_string())),
        }
    }

    pub fn get_message(&self) -> String {
        self.message.borrow().clone()
    }

    pub fn set_message(&mut self, msg: &str) {
        *self.message.borrow_mut() = msg.to_string();
    }
}

fn main() {
    let p1 = Point::new(10, 20);
    let p2 = Point::new(30, 40);

    let rect = Rectangle::new(p1, p2);
    let a = rect.area();

    let mut res = Resource::new(100);
    *res.get_mut(0) = 42;

    let mut data = SharedData::new("Hello, World!");

    println!("Area: {}", a);
    println!("Resource[0]: {}", res.get(0));
    println!("Message: {}", data.get_message());
}
