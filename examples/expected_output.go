// Expected Go output for simple_class.cpp

package main

import "fmt"

type Point struct {
    X int32
    Y int32
}

func NewPoint(x, y int32) Point {
    return Point{X: x, Y: y}
}

func (p *Point) GetX() int32 {
    return p.X
}

func (p *Point) GetY() int32 {
    return p.Y
}

func (p *Point) SetX(newX int32) {
    p.X = newX
}

func (p *Point) SetY(newY int32) {
    p.Y = newY
}

func (p *Point) DistanceSquared() int32 {
    return p.X*p.X + p.Y*p.Y
}

func (p *Point) Translate(dx, dy int32) {
    p.X += dx
    p.Y += dy
}

type Rectangle struct {
    TopLeft     Point
    BottomRight Point
}

func NewRectangle(tl, br Point) Rectangle {
    return Rectangle{
        TopLeft:     tl,
        BottomRight: br,
    }
}

func (r *Rectangle) Width() int32 {
    return r.BottomRight.GetX() - r.TopLeft.GetX()
}

func (r *Rectangle) Height() int32 {
    return r.BottomRight.GetY() - r.TopLeft.GetY()
}

func (r *Rectangle) Area() int32 {
    return r.Width() * r.Height()
}

type Resource struct {
    Data []int32
    Size uint
}

func NewResource(n uint) *Resource {
    return &Resource{
        Data: make([]int32, n),
        Size: n,
    }
}

func (r *Resource) Get(index uint) int32 {
    return r.Data[index]
}

func (r *Resource) Set(index uint, value int32) {
    r.Data[index] = value
}

func (r *Resource) GetSize() uint {
    return r.Size
}

type SharedData struct {
    Message string
}

func NewSharedData(msg string) *SharedData {
    return &SharedData{
        Message: msg,
    }
}

func (s *SharedData) GetMessage() string {
    return s.Message
}

func (s *SharedData) SetMessage(msg string) {
    s.Message = msg
}

func main() {
    p1 := NewPoint(10, 20)
    p2 := NewPoint(30, 40)

    rect := NewRectangle(p1, p2)
    a := rect.Area()

    res := NewResource(100)
    res.Set(0, 42)

    data := NewSharedData("Hello, World!")

    fmt.Printf("Area: %d\n", a)
    fmt.Printf("Resource[0]: %d\n", res.Get(0))
    fmt.Printf("Message: %s\n", data.GetMessage())
}
