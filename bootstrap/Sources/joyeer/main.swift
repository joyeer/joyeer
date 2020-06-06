if CommandLine.arguments.count > 1 {
    let file = CommandLine.arguments[1]
    let lexParser = try LexParser(path: file)
    lexParser.parse()
}
