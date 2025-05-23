package clang

import (
	cbase "github.com/jurgen-kluft/cbase/package"
	"github.com/jurgen-kluft/ccode/denv"
	centry "github.com/jurgen-kluft/centry/package"
	cunittest "github.com/jurgen-kluft/cunittest/package"
)

// GetPackage returns the package object of 'clang'
func GetPackage() *denv.Package {
	// Dependencies
	unittestpkg := cunittest.GetPackage()
	entrypkg := centry.GetPackage()
	basepkg := cbase.GetPackage()

	// The main (clang) package
	mainpkg := denv.NewPackage("clang")
	mainpkg.AddPackage(unittestpkg)
	mainpkg.AddPackage(entrypkg)
	mainpkg.AddPackage(basepkg)

	// 'clang' library
	mainlib := denv.SetupCppLibProject("clang", "github.com\\jurgen-kluft\\clang")
	mainlib.AddDependencies(basepkg.GetMainLib()...)

	// 'clang' unittest project
	maintest := denv.SetupDefaultCppTestProject("clang_test", "github.com\\jurgen-kluft\\clang")
	maintest.AddDependencies(unittestpkg.GetMainLib()...)
	maintest.AddDependencies(entrypkg.GetMainLib()...)
	maintest.Dependencies = append(maintest.Dependencies, mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)

	return mainpkg
}
