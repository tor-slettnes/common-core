C++ Style Guide
===============

This document descrives recommended styles and conventions for C++ code.  This is loosely modeled after the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) at the time of writing.   Some of the main goals behind this guide include:

* **Pulling its weight**. The benefit of a style guide must outweigh the hassle it imposes on developers to remember and adhere to it. Rules should follow common sense and be beneficial to the overall health of the code base, not arbitrary and burdensome.

* **Consistency**. Code should follow familiar conventions across teams, products, and with common conventions within the C++ community at large.

* **Optimize for reader, not writer**. Hopefully your code will continue to live for quite some time, and likely, more total time will be spent reading it than writing it:

  - Be explicit at call site: Provide context for the reader by adopting strong typing, fully qualified identifiers, explicit scoping, and input/output argument conventions as described below.

  - Use "principle of least astonishment": When something surprising or unusual is happening in a snippet of code (for example, transfer of pointer ownership), leaving textual hints for the reader at the point of use is valuable (`std::unique_ptr<>` demonstrates the ownership transfer unambiguously at the call site).


## <a name="code-compatibility">Code Compatibility</a>

### C++ Version

Currently, code should target C++17, i.e. should not use C++20, C++23, or C++26 features.  Our oldest supported target environment is Ubuntu 22.04.

### Platform specific code

Code that is platform specific (e.g., POSIX-specific syntax, or depending on systems-specific features such as D-Bus or Network Manager) should as much as possible be abstracted out to a platform-agnostic interface with platform-specific implementations.  Examples of this include the [platform abstractions](../../src/core/cpp/inner-core/common/platform/) with corresponding [OS-specific implementations](../../src/core/cpp/inner-core/platforms/) within the [inner core](../../src/core/cpp/inner-core/) library.


## <a name="source-files">Source files</a>

### <a name="declaration-vs-definition">Declaration vs. Definition Files</a>

C++ filenames should follow these rules:

#### Header files

Header files should have a `.h++` suffix. These make up your API for any internal and external application code, and will contain:

 - Type declarations, class/structure declarations, method declarations,  enumerations, constant or inline expressions, and optionally definitions for some short inline methods or class/function templates

 - [Doxygen](https://www.doxygen.nl/manual/) compliant documentation

Header files should be self-contained - i.e. compile on their own:

 - They must contain a header guard (e.g. `#pragma once`) to allow direct and indirect inclusion in multiple nested header/source file files

 - They must include any other header files (our own, third-party, system, or C++ STL) on which they depend - i.e. they must *not* depend on such files being included externally at the call site.

#### Inline definition files

Longer inline definitions (such as class/function templates) may be separated out from header files into corresponding `.i++` files.  These will normally be included near the bottom of a corresponding header file, where they are declared.


#### Definition files (or "source files")

Definition files should have a `.c++` suffix.  Generally speaking these contain the "meat" that actually gets compiled into machine code: Non-inline function/method definitions (including object constructors/destructors), static data members, etc.

#### A side note on templates

Please note that templates are not class or function declarations; they are merely inputs to the compiler for defining a class or function based on template arguments provided at the call site.  This means that they must always be declared inline: defining them in a `.c++` file (like a regular function) will *not* turn them into a linkable object that can then be used in another `.c++` file.

It is however acceptable to define *private* template functions and classes within the sole `.c++` file where it is used.


### <a name="include-statements">Include statements</a>

* Generally, do not rely on not rely on transitive inclusions.

  ```c++
  #include <filesystem>

  // Likely the above already includes <string>, but even so there is no
  // guarantee that this holds true across OS platforms and toolchains.
  // If we make direct use of `std::string`, also include this:
  #include <string>
  ```

  - You might skip some common ones such as `#include <string>` or `#include <memory>` in case where it is obvious that these _must_ be already included; for instance

    ```c++
    #include "types/create-shared.h++"  // clearly depends on `std::shared_ptr<>`
    //#include <memory>                 // can be omitted
    ```

* Order `#include` statements from "top" to "bottom":

   - Within a `.c++` file, start with the corresponding `.h++` file
   - Any other `.h++` file in the same folder
   - Any other `.h++` files from the same application/library (e.g. base classes, common types)
   - Any generated header files for your application (e.g. ProtoBuf, IDL, ...)
   - Shared `.h++` files: [mantle](../../src/mantle), [outer-core](../../src/core/cpp/outer-core), [inner-core](../../src/core/cpp/inner-core).
   - Third-party include files, e.g. `grpc++/channel.h`
   - OS/System files (e.g. `fcntl.h`) - but remember to abstract out platform specific code as described [above](#code-compatibility).
   - C++ STL modules, e.g. `iostream` or `cstdio`.


* Use `#include "NAME"` for include files within/relative to workspace, `#include <NAME>` for external files:

   ```c++
   // Use `#include ""` for files relative to your workspace
   #include "my-header-file.h++"    // First party file
   #include "generated-file.pb.h"   // Generated within project
   #include "types/value.h++"       // File from common repository (submodule)

   // Use `#include <>` for files from system-wide include dirs
   // (also those obtained from `pkg-config --cflags PACKAGENAME`)
   #include <grpc++/grpc++.h>       // Third-party, installed under `/usr/include` on POSIX
   #include <stdio.h>               // System library
   #include <filesystem>            // C++ STL
   ```


## <a name="coding-conventions">Coding Conventions</a>


### <a name="explicit-scoping">Use explicit scoping syntax at call site</a>

Consider this example:

  ```c++
  namespace my_outer_namespace::my_inner_namespace
  {
      void MyClass::print_something()
      {
          //
          // Insert large chunk of code here
          //
          std::cout << some_variable << std::endl;
      }
  }
  ```

From this call site it's not clear whether the dominant `some_variable` is originates locally within the method (well it's clear here but let's say you write a 100-line spaghetti method), the object instance, directly within `my_inner_namespace`, within `my_outer_namespace`, or in the global scope.

Following the principle of optimizing for readability, we make use of language features and syntax (such as C++ namespaces, `ClassName::`, `this->`) for scoping at the call site.

#### Use `this->` to refer to instance variables and methods

Languages like Python enforce this syntactically (using `self.` by convention), so the concept should be familiar.

  ```c++

  std::string MyClass::my_name() const
  {
      return "Count Olaf";
  }

  void MyClass::say_my_name_bad() const
  {
      std::cout << "My name is " << my_name() << std::endl;  // Bad
  }

  void MyClass::say_my_name_good() const
  {
      std::cout << "My name is " << this->my_name() << std::endl; // Good
  }
  ```

#### Use `ClassName::` (or `This::` alias) to refer to class variables and methods

  ```c++

  class MyClass
  {
     using This = MyClass;

  public:
     inline static void say_my_name_bad()
     {
         std::cout << my_name() << std::endl;       // Bad
     }

     inline static void say_my_name_good()
     {
         std::cout << This::my_name() << std::endl; // Good
     }

  private:
     inline static std::string my_name()
     {
         return "Count Olaf II";
     }
  };
  ```

#### Use explicit namespaces

Avoid global `using namespace` declarations, especially in header files. Not only does this make it harder to trace back the origins of identifiers; it may also introduce ambiguity for symbols defined in multiple namespaces.

  ```c++
  // Bad: implicit namespaces
  using namespace std;
  using namespace core::str;

  // ...

  // Here, it's not immediately obvious where `tolower()` comes from.
  string lowercase_string = tolower("MiXeD CaSe StriNG");


  cout << lowercase_string
       << endl;
  ```


  ```c++
  // Better: Explicit namespaces
  std::string lowercase_string = core::str::tolower("MiXeD CaSe StriNG");
  std::cout << lowercse_string
            << std::endl;
  ```

#### Use `::` for identifiers in global namespace, such as C functions:

  ```c++
  using FileDescriptor = int;
  FileDescriptor fd = ::open(argv[1], O_RDONLY);
  ```

#### Unqualified identifiers

The following types of identifiers may be used unqualified:

* Local variables, defined within the function body where they are used
* Preprocessor macros, e.g. `logf_info()`
* Preprocessor symbols in `ALL_CAPS`. (Deprecated.  New code should instead use `const` or `constexpr` expression within a more specific namespace).



#### Avoid Pseudo Hungarian Notation

One particularly stubborn disease that has been plaguing C++ code particularly in Microsoft/Windows environments is the use of "Pseudo Hungarian Notation" in identifier names, such as an `m_` prefix to denote an instance variable, `p_` to denote a pointer, or a `dw` prefix to denote the data type "double word".  While there's an interesting story behind this involving misunderstandings and revolts (see [Making wrong code look wrong](https://www.joelonsoftware.com/2005/05/11/making-wrong-code-look-wrong/)), the fact remains that given such conventions are confusing and unnecessary.  Instead, use conventions outlined above with regards to explicit typing and scoping.

One possible exception is the use of a `_` suffix to denote private identifiers within a class.  This is sometimes useful to avoid conflicts with similarly-named accessor methods.



### <a name="explicit-data-types">Use explicit data types</a>

Rather than using generic data types such as `int` or `std::string` for things that have a more specific meaning, create type aliases where it improves readability of your code.

Consider this example:

  ```c++
  // Not great
  std::unordered_map<std::string, AccessPointData> accesspoint_map;

  // Better
  using SSID = std::string;
  std::unordered_map<SSID, AccessPointData> accesspoint_map;

  // Better yet
  using SSID = std::string;
  using AccessPointMap = std::unordered_map<SSID, AccessPointData>;
  AccessPointMap accesspoint_map;
  ```

#### When to use (and not use) `auto`

The `auto` keyword is sometimes misunderstood by people new to C++. It does *not* declare a dynamic data type (as identifiers in Python), nor some sort of of variant/"any" type.  Instead, it allows the compiler to figure out the data type based on context.   As such, these two statements are equivalent:

  ```c++
  std::shared_ptr<MyType> my_ref = std::make_shared<MyType>(); // Explicit type declaration
  ```

  ```c++
  auto my_ref = std::make_shared<MyType>(); // Allow the compiler to figure out the type
  ```

In this example the type of `my_ref` is clear, either way. Other times, the use of `auto` can obfuscate the code (and you'll be back to Python before you know it):

  ```c++
  auto result = my_object.get_result();
  // What is result now?  What can we do with it?  Not clear from this call site.
  ```

As a general principle, using `auto` is acceptable (and even encouraged) when

* The type is clear from the statement (e.g. the template argument to `std::make_shared<>` above)

* The type is "complex, but well known", e.g.

    ```c++
    std::unordered_map<std::string, SomeType> my_map;
    SomeType some_value;

    // Explicit, but complex and not particularly readable
    if (std::unordered_map<std::string, SomeType>::node_type nh = my_map.extract("Some key"))
    {
        some_value = nh.mapped();
    }

    // Acceptable, and well documented; see:
    // [std::unordered_map<>::extract()](https://en.cppreference.com/w/cpp/container/unordered_map/extract)
    if (auto nh = my_map.extract("Some key"))
    {
        some_value = nh.mapped();
    }
    ```



### <a name="function-declarations">Function Declarations</a>

This section pertains to both standalone functions and methods within a class.

#### Naming

Function names shall comprise lowercase characters, with underscore (`_`) to bind multiple words 

#### Function Arguments

For people new to C++, one of the most confounding aspects is the different ways of passing arguments to (or from) functions. If you come from Python, Java, or C# background you'll have been exposed to *mutable* vs. *immutable* data types (e.g. lists vs. tuples in Python), but not pointers.  If you come from a C background you're all about pointers all the time, but you may be stumped by the notion of argument *references*.  Rest assured, there's a little bit here for everyone.

There are multiple different ways of passing in arguments to a function in C++:

  ```c++
  void my_function(
      std::string arg1,        // by value; the function receives a copy of `arg1`
      std::string &arg2,       // by (`lvalue`) reference; the function can modify the caller's copy
      const std::string &arg3, // by constant reference; the function can view the caller's copy
      std::string &&arg4,      // by `rvalue` reference; the function takes a transient value
      std::string *arg5,       // by pointer; the function receives the memory location of the value
      const std:string *arg6); // by const pointer; same, but the function cannot modify the value
  ```

For more information about the different reference types, see [C++ Value Categories](https://en.cppreference.com/w/cpp/language/value_category).

The corresponding call site may look something like this:

  ```c++
  std::string my_arg1 = "first argument";
  std::string my_arg2 = "second argument";
  std::string my_arg3 = "third argument";
  std::string my_arg5 = "fifth argument";
  std::string my_arg6 = "sixth argument";

  my_function(my_arg1,            // (1) by value; my_arg1 remains unmodified
              my_arg2,            // (2) by lvalue reference; my_arg2 *may* be modified
              my_arg3,            // (3) by constant reference; my_arg3 remains unmodified
              "fourth argument",  // (4) by rvalue reference; argument is "moved" to function
              &my_arg5,           // (5) by pointer; a modification is expected
              &my_arg6);          // (6) by pointer; argument cannot be modified
  ```

From the perspective of the call site, we note `my_arg5` and `my_arg6` look as if they are subject to modification (given their ampersand prefix). On the other hand, we do not expect `my_arg1`, `my_arg2` or `my_arg3` to be modified.  We also note that this is a bit different from reality.

Thus, in accordance with the principle of readability of the call site, we adopt these rules:

* Input-only arguments may be passed by value (1), constant reference (3), or rvalue reference (4).
   - We do _not_ pass inputs as `const` pointers (6). Rather, we use a constant reference, with a `*` prefix at the call site.

* Outputs and combined input/output arguments are passed as pointers (5), making it obvious that these may/will be modified.
   - We do _not_ pass output or I/O arguments by reference (2), as looking at the call site we would not expect these to change.

Clearly these rules apply only to our own software interfaces, not third-party libraries such as the C++ STL or gRPC.




### <a name="data-types">Classes and Data Types</a>

#### Naming

Class names and type aliases shall be written in PascalCase, where the first letter of each word is capitalized and with no separator between words.


#### `struct` vs. `class`

In C++, the `class` and `struct` keywords are essentially identical (except that unless declared otherwise, `class` members are private while `struct` members are public).  However, because `struct` is also a reserved keyword in C, it is commonly used to hold only variables/data, and not members.

We adopt this same convention, which means we apply stricter criteria to `struct` types than `class` types. Specifically:

* A `struct` type should have no methods (other than possible accessor method).  In particular it should have no constructor or destructor.

* A `struct` type should not have any `virtual` methods, nor be subclassed.

The above allows for C compatible (and frequently convenient) syntax like the following:

  ```c++
  struct MyStruct
  {
      std::string my_text;
      int my_num = 0;
  }


  MyStruct my_var = {.my_text = "The meaning of Life", .my_num = 42};
  ```


#### Class Declarations

Class declarations will generally follow variants of this pattern:

  ```c++
  namespace some_namespace
  {
      class ClassName : public SuperClass
      {
          using This = ClassName;
          using Super = SuperClass;

      public:
          ClassName(Type arg, ...);  // constructor
          virtual ~ClassName();      // destructor

      public:
          // Public methods

      protected:
          // Protected methods

      private:
          // Private methods

      private:
          // Private data
      };
  }
  ```

Of note:

* Private `This` and possibly `Super` type aliases (for readability)
* Methods before data
* `public` before `protected` before `private`


#### Methods

Here we use *method* to refer to a function defined within a class.  There are two subtypes:

* A *static method* does not belong to a specific object instance (and cannot refer to `this`).  A static method declaration is prefixed with the keyword `static`.

* An *instance method* belongs to an object instance, and can be further classified with the following decorators:

  - A `virtual` prefix to indicate that this is a polymorphic method, which may be overriden in subclasses.  Specifically, an object reference to a derived type will invoke the overridden method rather than that this (base) method.

  - A `= 0` suffix turns this into a *pure virtual* method, with no implementation in this (base) class.  Implicitly this turns the class into an *abstract class*, which cannot be instantiated other than through a subclass that overrides this method.

  - An `override` suffix indicates that this was declared as a virtual method in the base class. Some compilers such as `g++` will implicitly override virtual methods by the same name, while others such as `clang` will complain without explicitly calling this out with the `override` keyword.  As such, we require this.

  - A `const` suffix indicates that this method does not alter the object instance (and specifically any instance variables) in any way.  This is required in order to allow this method to be invoked from `const` class instance.

    - Note that this is different from a `const` prefix in the declaration, which indicates that the return type is a constant value (or reference).



### <a name="namespaces">Namespaces</a>

We use namespaces to group related code, frequently in a hierarchical fashion.  This allows the "base name" of identifiers to be short and succinct, without the risk of conflicts with similarly-named identifiers from other namespaces.  As discussed in the section on [explicit scoping](#explicit-scoping) above, we use fully scoped names at the call site.


## <a name="formatting">Code Formatting</a>

Now for everyone's favorite topic: Code formatting.  Once again, the emphasis should be on readability and consistency.

* Each indentation level is 4 spaces.

* Tabs are strictly forbidden. Their use will be prosecuted to the maximum extent of the law.

  - Thanks to Borland there is no universal interpretation of their meaning.

  - Although there are interesting perspectives and ideas behind their use (e.g. [Indent with tabs, align with spaces](https://dmitryfrank.com/articles/indent_with_tabs_align_with_spaces)), applications of such approaches tend to be sporadic and inconsistent, especially as new developers come onboard.

* Do not be afraid of vertical space.
   - Add empty lines between classes, functions, and logical code blocks.

* We have no strict limit on the length of each line. Instead, you should start feeling uncomfortable with lines exceeding 80 characters, and even more so beyond 96 characters.

  - Split up long argument lists to one argument per line (both when declaring/defining functions and at the call site where they are invoked):

      ```c++
      std::string wrap(
          const std::string &input,
          size_t start_column = 0,
          size_t left_margin = 24,
          size_t right_margin = 80,
          bool keep_empties = false);
      ```

  - Consider splitting up complex statements at logical points.  For example:

      ```c++
      // Ternary statement, single line
      return condition ? true_value : false_value;

      // Ternary statement, two lines
      return condition ? true_value
                       : false_value;

      // Ternary statement, three lines
      return condition
          ? true_value
          : false_value;
      ```


* Left curly braces begin on their own line, in line with the parent statement and the closing right curly brace:

    ```c++
    // Wrong: Braces do not line up.  Code looks crammed.
    void my_function(bool condition) {
        if (condition) {
            do_something();
        }
    }

    // Wrong, and inconsistent:
    void my_function(bool condition)
    {
        if (condition) {
            do_something();
        }
    }

    // Correct:
    void my_function(bool condition)
    {
        if (condition) 
        {
            do_something();
        }
    }
    ```

* Use curly braces for nested blocks, even where it contains a single statement:

    ```c++
    // Wrong:
    if (condition)
        statement;

    // Correct:
    if (condition)
    {
        statement;
    }
    ```

* *Do not* insert space characters before opening parentheses of argument lists

  - This holds for declarations/definitions as well as call site.

      ```c++
      /// Incorrect: Space before argument list
      void my_function (int arg1, int arg2);

      /// Correct: No space before argument list:
      void my_function(int arg1, int arg2);
      ```


* *Do* insert a space character before opening parentheses following `if`, `for`, `while`, `switch`.

    ```c++
    /// Incorrect: No space before condition:
    if(condition)
    {
        // ...
    }

    /// Correct: Space before condition:
    if (condition)
    {
        // ...
    }
    ```


## <a name="reference">C++ documentation</a>

You'll find by far the most comprehensive standard C++ reference at [cppreference.com](https://cppreference.com/).  You can also access this documentation offline in the [DevHelp](https://wiki.gnome.org/Apps/Devhelp) application on your Linux desktop by installing the package `cppreference-doc-en-html` (for [Debian](https://packages.debian.org/search?keywords=cppreference&searchon=names&suite=all&section=all) or [Ubuntu](https://packages.ubuntu.com/search?keywords=cppreference&searchon=names&suite=all&section=all)).

Slightly more accessible documentation for people new to C++ can be found at [cplusplus.com](https://cplusplus.com/).
