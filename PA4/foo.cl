Class A {
  a : SELF_TYPE <- (new SELF_TYPE);
  f(c : SELF_TYPE, d : Int) : Int { {1;2;} };
};
Class B inherits A {
  b : Int <- case a of d : Int => d; e : Int => e; esac;
  c : Int;
};
