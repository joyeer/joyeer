//
//  StringReader.swift
//  joyeer
//
//  Created by Joyeer on 2020/6/4.
//

import Foundation

public class StringReader {
    private let characters:[Character]
    public var pos = -1

    public var count :Int {
        return characters.count
    }
    
    public init(_ descriptor:String) {
        self.characters = descriptor.map { $0 }
    }

    public func hasNext() -> Bool {
        return (pos + 1) < self.characters.count
    }

    public func next() -> Character? {
        pos += 1
        return cur()
    }

    public func prevous() -> Character? {
        pos -= 1
        return cur()
    }

    public func cur() -> Character? {
        if pos < 0 {
            return nil
        }
        if pos >= self.characters.count {
            return nil
        }
        return self.characters[pos]
    }

    public func substring(startAt start:Int, endAt end:Int) -> String {
        let subarray = self.characters[start...end]
        return String(subarray)
    }
    

}
