package main

import "fmt"

// #cgo LDFLAGS: -lgoc
// #include <goc.h>
import "C"

func main() {
	fmt.Println("sum 5+2:", C.Sum(5, 2))
	fmt.Println("mul 5*2:", C.Mul(5, 2))
	fmt.Println("extract 5-2:", C.Extract(5, 2))
}
