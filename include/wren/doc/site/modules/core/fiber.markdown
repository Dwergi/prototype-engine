^title Fiber Class

A lightweight coroutine. [Here][fibers] is a gentle introduction.

[fibers]: ../../concurrency.html

### Fiber.**new**(function)

Creates a new fiber that executes `function` in a separate coroutine when the
fiber is run. Does not immediately start running the fiber.

    :::wren
    var fiber = Fiber.new {
      System.print("I won't get printed")
    }

## Static Methods

### Fiber.**current**

The currently executing fiber.

### Fiber.**suspend**()

Pauses the current fiber, and stops the interpreter. Control returns to the
host application.

To resume execution, the host application will need to invoke the interpreter
again. If there is still a reference to the suspended fiber, it can be resumed.

### Fiber.**yield**()

Pauses the current fiber and transfers control to the parent fiber. "Parent"
here means the last fiber that was started using `call` and not `run`.

    :::wren
    var fiber = Fiber.new {
      System.print("Before yield")
      Fiber.yield()
      System.print("After yield")
    }

    fiber.call()                //> Before yield
    System.print("After call")  //> After call
    fiber.call()                //> After yield

When resumed, the parent fiber's `call()` method returns `null`.

If a yielded fiber is resumed by calling `call()` or `run()` with an argument,
`yield()` returns that value.

    :::wren
    var fiber = Fiber.new {
      System.print(Fiber.yield()) //> value
    }

    fiber.call()        // Run until the first yield.
    fiber.call("value") // Resume the fiber.

If it was resumed by calling `call()` or `run()` with no argument, it returns
`null`.

If there is no parent fiber to return to, this exits the interpreter. This can
be useful to pause execution until the host application wants to resume it
later.

    :::wren
    Fiber.yield()
    System.print("this does not get reached")

### Fiber.**yield**(value)

Similar to `Fiber.yield` but provides a value to return to the parent fiber's
`call`.

    :::wren
    var fiber = Fiber.new {
      Fiber.yield("value")
    }

    System.print(fiber.call()) //> value

## Methods

### **call**()

Starts or resumes the fiber if it is in a paused state.

    :::wren
    var fiber = Fiber.new {
      System.print("Fiber called")
      Fiber.yield()
      System.print("Fiber called again")
    }

    fiber.call() // Start it.
    fiber.call() // Resume after the yield() call.

When the called fiber yields, control is transferred back to the fiber that
called it.

If the called fiber is resuming from a yield, the `yield()` method returns
`null` in the called fiber.

    :::wren
    var fiber = Fiber.new {
      System.print(Fiber.yield())
    }

    fiber.call()
    fiber.call() //> null

### **call**(value)

Invokes the fiber or resumes the fiber if it is in a paused state and sets
`value` as the returned value of the fiber's call to `yield`.

    :::wren
    var fiber = Fiber.new {
      System.print(Fiber.yield())
    }

    fiber.call()
    fiber.call("value") //> value

### **isDone**

Whether the fiber's main function has completed and the fiber can no longer be
run. This returns `false` if the fiber is currently running or has yielded.

### **transfer**()

**TODO**

### **transfer**(value)

**TODO**

### **transferError**(error)

**TODO**
