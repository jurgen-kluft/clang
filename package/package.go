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
	mainpkg := denv.NewPackage("github.com\\jurgen-kluft", "clang")
	mainpkg.AddPackage(unittestpkg)
	mainpkg.AddPackage(entrypkg)
	mainpkg.AddPackage(basepkg)

	// 'clang' library
	mainlib := denv.SetupCppLibProject(mainpkg, "clang")
	mainlib.AddDependencies(basepkg.GetMainLib()...)

	// 'clang' unittest project
	maintest := denv.SetupCppTestProject(mainpkg, "clang_test")
	maintest.AddDependencies(unittestpkg.GetMainLib()...)
	maintest.AddDependencies(entrypkg.GetMainLib()...)
	maintest.AddDependency(mainlib)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddUnittest(maintest)

	return mainpkg
}
