# Project 3 Design Log
**Author:** <Melina Lukasik> · **P2 code used:** <instructor reference>
**Tools used:** <cppreference and stackexchange were consulted (as in, I looked at posts that were already made in the past) so I could get a get an idea of how std::vector, lambdas, and a few other topics work>

# MEL!!! REMINDER TO PUT THEM ^^^ BACK AND THEN FIGURE OUT HOW TO DO THE CMAKE THING


## 1. Hash table
	 
	The reason I chose chaining over open addressing is that I wanted to prevent the find() function from becoming excessively time-consuming. With open addressing, the find() function first tries to find a match for the tool name in the hashed bucket,  then continues looking through the table until it reaches a blank; only then can it return that it found nothing. Meanwhile, with chaining, the destination of the hash function is a linked list containing everything with the same hash value. If a match isn't in that, no more buckets need to be traversed before the function can return that its search was for naught.
	 
	I chose a load-factor threshold of 0.8. This was an arbitrary choice from within the recommended threshold of 0.75-1.0.
	
				Bucket Count: 16384
				Load Factor: 0.610352
				Max Probe Length: 6

				Weak Hash Bucket Count: 16384
				Weak Hash Load Factor: 0.0273438
				Weak Hash Max Probe Length: 24

	The add() function is amortized O(1). It takes the hash function of the tool given and then adds the tool to the linked list at that address (after checking that something sharing a name isn't already in there). Because it checks the entire linked list, the worst-case (everything hashing to the same bucket) is O(n), but thanks to resizing at load-factor and maximum-probe-length thresholds any one bucket is unlikely to have more than a few tools in it. The resizing operation is O(n), but it is infrequent and doesn't affect the overall amortized cost.

## 2. Pointer stability

	The Tool* is linked to an object pointed to by a unique pointer, which is moved rather than copied during a resize. Since the object itself is never cloned or deleted (all that happens is that the unique pointer pointing to it is moved), the Tool* pointer pointing to the Tool object is never invalidated, because the object it's pointing to is still in the same place.

## 3. Calculator trace

		2 * (3 + 4) - 5
	 
	 ADDING TOKENS TO STACKS
	 Token 01: 2.0 -> values
	 Token 02: '*' -> (no operator on top of ops so) -> ops
	 Token 03: '(' -> (opening parenthesis so) -> ops
	 Token 04: 3.0 -> values
	 Token 05: '+' -> (last operator is '(' so) -> ops
	 Token 06: 4.0 -> values
	 Token 07: ')' -> (closing parenthesis so) ->
					APPLY: 3.0 + 4.0. : Replace 3.0 with result (7.0) : pop '('
	 Token 08: '-' -> (last operator is '*' (precedence >= '-') so) ->
					APPLY: 2.0 * 7.0 : Replace 2.0 with result (14.0) ->
					now put '-' -> ops
	 Token 09: 5.0 -> values
	 END OF TOKENS REACHED so APPLY the rest
					APPLY: 14.0 - 5 : Replace 14.0 with result (9.0)
	 NO OPERATORS LEFT so RETURN ONLY MEMBER OF values
					RETURN: 9.0
		
		-> 9.0
	 
	Stack timeline:
	 
	values:{2.0} ops:{}
	values:{2.0} ops:{'*'}
	values:{2.0} ops:{'*', '('}
	values:{2.0, 3.0} ops:{'*', '('}
	values:{2.0, 3.0} ops:{'*', '(', '+'}
	values:{2.0, 3.0, 4.0} ops:{'*', '(', '+'}
	values:{2.0, 7.0} ops:{'*'}
	values:{14.0} ops:{'-'}
	values:{14.0, 5.0} ops:{'-'}
	values:{9.0} ops:{}
	
	
## 4. Errors as values
	
	Harnesses cannot always expect all of the tools it has access to to work every time. Sometimes, these failures can also provide useful information.
	For example:
		If a user prompts an LLM to perform '1/0', the harness will perform the operation and it will fail. There's no reason for the entire operation to fall apart because that failed. The error type can be passed back to some sort of reasoning module (forgive me for not knowing in depth how AI work) and the LLM can give a response telling the user that dividing by zero is generally not considered a valid operation among the Reals, which is informative. If the entire harness halted for that small error (that doesn't impact the program's continued running), no useful information could be gleamed from it.
		
	An exception would be the right choice for a situation where the error would put the core program at risk or jeapordize its functionality.

## 5. What I would design differently

	I would try to find some way to lessen the amount of ops.empty() checks in calculator_eval.cpp, just because it bloats the code without being complex enough to deserve its own helper function (calling which would use almost the same amount of space).
	I would also polish my test_p3 a little more since the code is rather messy right now.