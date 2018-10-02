FileStream fileIn: 'ModifyOrderedCollection.st'.


Object subclass: #LittleStackMachine.
LittleStackMachine instanceVariableNames: 'stack data code'.
LittleStackMachine extend [
    init [
        code := OrderedCollection new.
        data := OrderedCollection new.
        ^self
    ]
    run: input [
        stack := OrderedCollection new.
        input collect: [:next | self processNumber: next].
        ^stack
    ]
    processNumber: value [
        ((value class) = NodeNumber)
            ifTrue: [ stack add: value ]
            ifFalse: [ self processOp: value ]
    ]
    processOp: op [
        ((op class) = NodeOp)
            ifTrue: [
                self processAdd: op;
                processSub: op;
                processMul: op;
                processDiv: op.
            ]
            ifFalse: [ self processAssignVariable: op ]
    ]
    processAssignVariable: variable [
        ((variable class) = AssignVariable)
            ifTrue: [
                (variable value) > (data size)
                    ifTrue: [
                        data := data resetSize: (variable value).
                    ].
                data at: (variable value) put: (stack last copy).
            ]
            ifFalse: [ self processAssignClosure: variable ]
    ]
    processAssignClosure: closure [
        ((closure class) = NodeClosure)
            ifTrue: [
                (closure value) > (code size)
                    ifTrue: [
                        code := code resetSize: (closure value).
                    ].
                code at: (closure value) put: (closure body copy).
                stack add: (NodeNumber set: (closure value))
            ]
            ifFalse: [ self processExecuteClosure: closure]
    ]
    processExecuteClosure: closure [
        |executeCode|
        (closure class = ExecuteClosure)
            ifTrue: [
                executeCode := (code at: (closure value)) copy.
                executeCode collect: [:current | self processNumber: current].
            ]
            ifFalse: [self processAnonymousExecuteClosure: closure]
    ]
    processAnonymousExecuteClosure: closure [
        (closure class = AnonymousExecuteClosure)
            ifTrue: [
                (closure value) collect: [:current | self processNumber: current].
            ]
            ifFalse: [self processParameter: closure]
    ]
    processParameter: parameter [
        (parameter class = NodeParameter)
            ifTrue: [
                (parameter value) > (data size)
                    ifTrue: [
                        data := data resetSize: (parameter value).
                    ].
                data at: (parameter value) put: (stack removeLast).
            ]
            ifFalse: [ self processMessage: parameter]
    ]
    processMessage: message [
        (message class = NodeMergedMessage)
            ifTrue: [
                stack add: (message body).
            ]
            ifFalse: [ self processVariable: message]
    ]
    processVariable: variable [
        ((variable class) = NodeVariable)
            ifTrue: [
                stack add: (data at: (variable value) copy).
            ]
            ifFalse: [ self processExpression: variable ]
    ]
    processExpression: value [
        ((value class) = NodeExpression)
            ifTrue: [ (stack removeLast) printNl ]
    ]
    processAdd: op [
        |r1 r2|
        ((op value) = '+')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 + r2))
            ].
        ^self
    ]
    processSub: op [
        |r1 r2|
        ((op value) = '-')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 - r2))
            ].
        ^self
    ]
    processMul: op [
        |r1 r2|
        ((op value) = '*')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 * r2))
            ].
        ^self
    ]
    processDiv: op [
        |r1 r2|
        ((op value) = '/')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 / r2))
            ].
        ^self
    ]
]