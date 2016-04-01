foreign class WrenTest {
	construct new( i, b ) {}

	foreign Integer
	foreign Integer=( rhs ) 

	foreign TestFunction()
	foreign TestFunctionArg( a, b )
	foreign TestFunctionRet()
}

class Test {
	construct new() {}

	call_cpp() {
		var test = WrenTest.new( 0, false )
		test.TestFunction()
		var ret = test.TestFunctionRet()

		System.print( "TestFunctionRet returned: " )
		System.print( ret )
		test.TestFunctionArg( 1, 2 )
	}
}

var test_var = Test.new()