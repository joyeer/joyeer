import Foundation

class LexParser {
    let reader: StringReader
    var lineNumber = 0
    var startAt = 0
    init(path: String) throws {
        let content = try String(contentsOf: URL(fileURLWithPath: path))
        reader = StringReader(content)
    }
    
    func parse() {
        while reader.hasNext() {
            let ch = reader.next()
            switch ch {
            case "\u{000A}":
                lineNumber += 1
                startAt = reader.pos
            case "\u{000D}":
                _ = tryEat("\u{000A}")
                lineNumber += 1
                startAt = reader.pos
            case "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "_":
                parseIdentifier()
            case "-":
                break
            case "+":
                break
            case "/":
                break
            case "=":
                break
            case "!":
                break
            case "*":
                break
            case "%":
                break
            case "0":
                break
            case "1", "2", "3", "4", "5", "6", "7", "8", "9":
                break
            default:
                break
            }
        }
    }
    
    private func parseIdentifier() {
        let startAt = reader.pos
        label: while reader.hasNext() {
            let ch = reader.next()
            switch ch {
            case "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "_",
                 "0", "1", "2", "3", "4", "5", "6", "7", "8", "9":
                continue
            default:
                _ = reader.prevous()
                break label
            }
        }
        
        let identifier = reader.substring(startAt: startAt, endAt: reader.pos)
        print(identifier)
        
    }
    
    private func tryEat(_ ch: Character) -> Bool {
        if reader.hasNext() {
            if reader.next() == ch {
                return true
            }
            _ = reader.prevous()
        }
        return false
    }
    
    private func parseLiteral() {
        
    }
}
