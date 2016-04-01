class Test {
	construct new() {}

	simple() {
		System.print( "test_simple called!" )
	}

	with_arg( a ) {
		System.print( a )
	}

	multiple_args( a, b ) {
		System.print( a )
		System.print( b )
	}

	with_return() {
		System.print( 5 )
		return 5
	}
}

var test_var = Test.new()