FileStream fileIn: 'Instance.st'.


Object subclass: #LittleStackMachine.
LittleStackMachine instanceVariableNames: 'stack data code instances currentInstance messageTable'.
LittleStackMachine class extend [
    createWithMessageTable: aTable [
        ^(super new initWithMessageTable: aTable)
    ]
]
LittleStackMachine extend [
    initWithMessageTable: aTable [
        messageTable := aTable.
        code := OrderedCollection new.
        data := OrderedCollection new.
        instances := OrderedCollection new.
        ^self
    ]
    getInstance: value [
        ^(instances at: value)
    ]
    createFunctionInstance: node with: code [
        |result aTypeName aSize|
        result := Instance createWithInfo: node andCode: code.
        instances add: result.
        ^(instances size)
    ]
    reset [
        stack := OrderedCollection new.
    ]
    run: input [
        input collect: [:current | self processNumber: current].
        ^stack
    ]
    processNumber: value [
        (value class = NodeNumber)
            ifTrue: [ stack add: value ]
            ifFalse: [ self processOp: value ]
    ]
    processOp: op [
        (op class = NodeOp)
            ifTrue: [
                self processAdd: op;
                processSub: op;
                processMul: op;
                processDiv: op.
            ]
            ifFalse: [ self processAssignVariable: op ]
    ]
    processAssignVariable: variable [
        (variable class = AssignVariable)
            ifTrue: [
                (variable value) > (data size)
                    ifTrue: [
                        data := data resetSize: (variable value).
                    ].
                data at: (variable value) put: (stack last copy).
            ]
            ifFalse: [ self processAssignFunction: variable ]
    ]
    processAssignFunction: node [
        |info key value executeCode|
        (node class = NodeFunction)
            ifTrue: [
                (node value) > (code size)
                    ifTrue: [
                        code := code resetSize: (node value).
                    ].
                info := node info.
                executeCode := node body.
                key := ((1 to: (info propertySize)) collect: [:current | 'value:']) join: ''.
                info addMethod: (messageTable findOrCreateBy: key).
                value := self createFunctionInstance: info with: executeCode.
                code at: (node value) put: value.
                stack add: (NodeNumber set: (node value))
            ]
            ifFalse: [ self processExecuteFunction: node]
    ]
    processExecuteFunction: function [
        |instanceValue key|
        (function class = ExecuteFunction)
            ifTrue: [
                instanceValue := (code at: (function value)) copy.
                currentInstance := instances at: instanceValue.
                key := stack removeLast.
                (currentInstance loadCode: key) collect: [:current | self processNumber: current].
            ]
            ifFalse: [self processAnonymousExecuteFunction: function]
    ]
    processAnonymousExecuteFunction: node [
        |info key value executeCode|
        (node class = AnonymousExecuteFunction)
            ifTrue: [
                info := node info.
                executeCode := node body.
                key := ((1 to: (info propertySize)) collect: [:current | 'value:']) join: ''.
                info addMethod: (messageTable findOrCreateBy: key).
                value := self createFunctionInstance: info with: executeCode.
                node setProcessed; set: value.
                currentInstance := instances at: value.
                key := stack removeLast.
                (currentInstance loadCode: key) collect: [:current | self processNumber: current].
                instances remove: currentInstance.
                currentInstance := nil.
            ]
            ifFalse: [self processParameter: node]
    ]
    processParameter: parameter [
        (parameter class = NodeParameter)
            ifTrue: [
                currentInstance at: (parameter value) put: (stack removeLast).
            ]
            ifFalse: [ self processMessage: parameter]
    ]
    processMessage: message [
        (message class = NodeMessage)
            ifTrue: [
                self run: (message body).
                stack add: (message value).
            ]
            ifFalse: [ self processProperty: message]
    ]
    processProperty: property [
        (property class = NodeProperty)
            ifTrue: [
                stack add: (currentInstance at: (property value) copy).
            ]
            ifFalse: [ self processVariable: property ]
    ]
    processVariable: variable [
        (variable class = NodeVariable)
            ifTrue: [
                stack add: (data at: (variable value) copy).
            ]
            ifFalse: [ self processExpression: variable ]
    ]
    processExpression: value [
        (value class = NodeExpression)
            ifTrue: [ (stack removeLast) printNl ]
    ]
    processAdd: op [
        |r1 r2|
        (op value = '+')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 + r2))
            ].
        ^self
    ]
    processSub: op [
        |r1 r2|
        (op value = '-')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 - r2))
            ].
        ^self
    ]
    processMul: op [
        |r1 r2|
        (op value = '*')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 * r2))
            ].
        ^self
    ]
    processDiv: op [
        |r1 r2|
        (op value = '/')
            ifTrue: [
                r2 := stack removeLast value.
                r1 := stack removeLast value.
                stack add: (NodeNumber set: (r1 / r2))
            ].
        ^self
    ]
]
