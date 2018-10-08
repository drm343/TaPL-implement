FileStream fileIn: 'ModifyOrderedCollection.st'.


Object subclass: #MergeMessage.
MergeMessage instanceVariableNames: 'tmp stack result previous'.
MergeMessage extend [
    run: input [
        result := OrderedCollection new.
        previous := nil.
        stack := input collect: [:current |
            (current class = AnonymousExecuteFunction)
                ifTrue: [
                    tmp := MergeMessage new.
                    tmp := tmp run: (current body).
                    current setBody: tmp.
                    current
                ]
                ifFalse: [current]
        ].
        stack := input collectWithNext: [:current :next |
            (current class = NodeAtomMessage)
                ifTrue: [previous := (NodeMergedMessage upgradeFrom: current and: next). previous copy]
                ifFalse: [
                    (previous class = NodeMergedMessage)
                        ifTrue: [previous := current. nil]
                        ifFalse: [previous := current. current copy]
                ]
        ].
        stack := (stack reject: [:current | current = nil]) copy.
        previous := nil.
        stack collect: [:current |
            (current class = NodeMergedMessage)
                ifTrue: [
                    (previous = nil)
                        ifTrue: [
                            previous := NodeMessage upgradeFrom: current.
                        ]
                        ifFalse: [
                            previous upgradeWith: current.
                        ].
                ]
                ifFalse: [
                    (previous = nil)
                        ifTrue: [
                            result add: current.
                        ]
                        ifFalse: [
                            result add: previous.
                            result add: current.
                            previous := nil.
                        ].
                ].
            current
        ].
        (previous = nil)
            ifFalse: [
                result add: previous.
                previous := nil.
            ].
        ^result
    ]
]
