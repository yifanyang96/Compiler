Class A {
    a : Int <- 1 + 2 * 5;
    f() : Int {a};
};
Class Main {
    main() : IO { (new IO).out_int((new A).f()) };
};