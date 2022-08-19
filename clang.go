package main

import (
	"github.com/jurgen-kluft/ccode"
	clang "github.com/jurgen-kluft/clang/package"
)

func main() {
	ccode.Generate(clang.GetPackage())
}
