package main

import (
	"github.com/jurgen-kluft/ccode"
	"github.com/jurgen-kluft/clang/package"
)

func main() {
	ccode.Init()
	ccode.Generate(clang.GetPackage())
}
