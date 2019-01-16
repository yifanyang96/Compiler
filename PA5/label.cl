Class A {
    a : Int <- 1;
    b : Int <- 2;
    c : Int <- if a<b then a else b fi;
    d : Int <- if a<b then b else a fi;
};
Class Main inherits A {
    main : Int { d };
};