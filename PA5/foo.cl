Class A {};
Class B {};
Class C {};
Class D inherits C {};
Class E inherits C {};
Class F inherits E {};
Class G {};
Class Main inherits A {
    main() : Int { 1 };
};