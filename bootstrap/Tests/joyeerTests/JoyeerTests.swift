import XCTest
import class Foundation.Bundle

final class JoyeerTests: XCTestCase {
    static var resourceDirectoryURL: URL {
        var resourceDirectoryURL = URL(fileURLWithPath: #file)
        resourceDirectoryURL.deleteLastPathComponent()
        resourceDirectoryURL.appendPathComponent("Resources")
        return resourceDirectoryURL
    }

    
    func testLexer() throws {
        var path = Self.resourceDirectoryURL
        path.appendPathComponent("list.joyeer")
        
        let lexer = LexParser(path: path.path)
        try lexer.parse()
    }
}
