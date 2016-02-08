^title Random Class

A simple, fast pseudo-random number generator. Internally, it uses the [well
equidistributed long-period linear PRNG][well] (WELL512a).

[well]: https://en.wikipedia.org/wiki/Well_equidistributed_long-period_linear

Each instance of the class generates a sequence of randomly distributed numbers
based on the internal state of the object. The state is initialized from a
*seed*. Two instances with the same seed generate the exact same sequence of
numbers.


It must be imported from the [random][] module:

    :::wren
    import "random" for Random

[random]: ../

## Constructors

### Random.**new**()

Creates a new generator whose state is seeded based on the current time.

    :::wren
    var random = Random.new()

### Random.**new**(seed)

Creates a new generator initialized with [seed]. The seed can either be a
number, or a non-empty sequence of numbers. If the sequnce has more than 16
elements, only the first 16 are used. If it has fewer, the elements are cycled
to generate 16 seed values.

    :::wren
    Random.new(12345)
    Random.new("appleseed".codePoints)

## Methods

### **float**()

Returns a floating point value between 0.0 and 1.0, including 0.0, but excluding
1.0.

    :::wren
    var random = Random.new(12345)
    System.print(random.float()) //> 0.53178795980617
    System.print(random.float()) //> 0.20180515043262
    System.print(random.float()) //> 0.43371948658705

### **float**(end)

Returns a floating point value between 0.0 and `end`, including 0.0 but
excluding `end`.

    :::wren
    var random = Random.new(12345)
    System.print(random.float(0))     //> 0
    System.print(random.float(100))   //> 20.180515043262
    System.print(random.float(-100))  //> -43.371948658705

### **float**(start, end)

Returns a floating point value between `start` and `end`, including `start` but
excluding `end`.

    :::wren
    var random = Random.new(12345)
    System.print(random.float(3, 4))    //> 3.5317879598062
    System.print(random.float(-10, 10)) //> -5.9638969913476
    System.print(random.float(-4, 2))   //> -1.3976830804777

### **int**(end)

Returns an integer between 0 and `end`, including 0 but excluding `end`.

    :::wren
    var random = Random.new(12345)
    System.print(random.int(1))    //> 0
    System.print(random.int(10))   //> 2
    System.print(random.int(-50))  //> -22

### **int**(start, end)

Returns an integer between `start` and `end`, including `start` but excluding
`end`.

    :::wren
    var random = Random.new(12345)
    System.print(random.int(3, 4))    //> 3
    System.print(random.int(-10, 10)) //> -6
    System.print(random.int(-4, 2))   //> -2
