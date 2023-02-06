var myexample = [
  [4, 5, 0],
  [8, 5, 0],
  [0, 1, 2],
];

function findSmallest(array) {
  var numberOfZeros = 0;
  for (var i = 0; i < array.length; i++) {
    for (var j = 0; j < array[i].length; j++) {
      if (array[i][j] == 0) {
        numberOfZeros++;
      }
    }
  }
  return numberOfZeros;
}
