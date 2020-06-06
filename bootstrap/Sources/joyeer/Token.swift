//
//  Token.swift
//  joyeer
//
//  Created by Joyeer on 2020/6/6.
//

import Foundation


struct Token {
    let line: Int
    let column: Int
    let type: TokenType
    let value: Any?
}

enum TokenType {
    case Keyword
    case Integer
    case Float
    case Literal
}
