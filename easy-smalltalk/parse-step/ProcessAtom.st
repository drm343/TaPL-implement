FileStream fileIn: 'ModifyOrderedCollection.st'.


Object subclass: #ProcessAtom.
ProcessAtom instanceVariableNames: 'result'.
ProcessAtom extend [
    run: input [
        result := input collectWithNext: [:current :next |
            ((next class) = NodeAssignment) & (current class = NodeAtom)
                ifTrue: [AssignVariable upgradeFrom: current]
                ifFalse: [current]
        ].
        result := result collect: [:current |
            ((current class) = NodeAtom)
                ifTrue: [NodeVariable upgradeFrom: current]
                ifFalse: [current]
        ].
        ^(result reject: [:next | (next class) = NodeAssignment])
    ]
]
