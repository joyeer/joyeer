import XCTest

import bootstrapTests

var tests = [XCTestCaseEntry]()
tests += bootstrapTests.allTests()
XCTMain(tests)
