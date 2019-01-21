Class A {
    a : Int <- 0;
    f() : Int {a};
};
Class B inherits A {
};
Class C inherits A {};
Class Main {
    c : Int <- case (new C) of x : A => x.f(); x : B => x.f() + 1; x : C => x.f() + 2; esac;
    main() : IO { (new IO).out_int(c) };
};