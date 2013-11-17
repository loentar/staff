#ifndef _test215_h_
#define _test215_h_

  struct TestPrec
  {
    // *precision: .0
    long double longDoubleValueNoPrec;
    // *precision: .0
    double doubleValueNoPrec;
    // *precision: .0
    float floatValueNoPrec;
    // *precision: 2.16
    long double longDoubleValue;
    // *precision: 2.12
    double doubleValue;
    // *precision: 2.4
    float floatValue;

    // *isAttribute: true
    // *precision: .0
    float attrFloatValueNoPrec;
    // *isAttribute: true
    // *precision: 2.4
    float attrFloatValue;
  };


#endif // _test215_h_
