//
//  Keyword.swift
//  joyeer
//
//  Created by Joyeer on 2020/6/6.
//

import Foundation

struct Keyword {
    static let `class` = "class"
    static let `struct` = "struct"
    static let `var` = "var"
    static let `let` = "let"
    static let `if` = "if"
    static let `else` = "else"
    static let `for` = "for"
    static let `while` = "while"
    static let `import` = "import"
    static let `func` = "func"
    static let `try` = "try"
    
    static let maps = [
        Self.`func`: Self.`func`,
        Self.struct: Self.struct
    ]
    
    static func isKeyword(_ keyword: string) ->  Bool {
        return Self.maps.contains(keyword)
    }
}

