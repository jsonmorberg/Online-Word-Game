    //Initialize Storage for networking composition and Trie for dictionary
    //Connection setup
    //  Accept TCP Connection 
    //  Fork
    //  Send Game Information and Start
    //
    //Game Start
    //  Start when there are two Players
    //  End on Premature Disconnection
    //
    //Rounds
    //  Generate Random Characters
    //  Send Round Information to Clients
    //  Loop Turns for each Player
    //
    //Turns
    //  Notify Player Turn
    //  Wait N Seconds for Word
    //  Receive Word (Or Lack of Word)
    //  Check validity of Word
    //      Valid Word
    //          *not been used
    //          *in dictionary
    //          *only uses board letters 
    //          Send Word to Next Player
    //      Invalid Word/Out of Time
    //          Next Player gains point
    //          Round Increments
    //Repeat 3 Times
    //Close Connection


//Functions
//loadTree: will add all the words from dictionary into tree
//createRandomCahrs: create random chars for round
//checkWord: checks if word is valid