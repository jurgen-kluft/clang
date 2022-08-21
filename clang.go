package main

import (
	"github.com/jurgen-kluft/ccode"
	"github.com/jurgen-kluft/clang/package"
)

func main() {
	ccode.Generate(clang.GetPackage())
}
