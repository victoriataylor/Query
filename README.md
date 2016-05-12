# Query

### Description
Query rebuilds an index from the previously built index.dat created by the indexer. It then asks the user to input key words. For each query,
it checks the index and retrieves and ranks the results based on the keywords and the operands AND and OR. If no operand is
inputted between keywords, the AND operand is assumed. Query then displays the list of results, returning the document with
the highest number of matches to the query string, then the second highest, third highest etc. 
it should load a previously generated index from the file system;

### Running Query
Parameter Input<br>
``./Query [INDEX_FILE] [TARGET_DIRECTORY]``

Example command input<br>
``query ../indexer/src/indexer.dat ../indexer/DIRECTORY``

For further documentation, see [here](./Query Documentation)
