/** {1 Mutable dynamic array} */;

type t('a, -'p);

/** The 'p type parameter is a phantom type parameter that represents the vector's mutability.
It is [[`R | `W]] for read-write vectors, [[`R]] for read-only vectors, or [[`W]] for write-only vectors. */;

/** The default growth rate of newly-created vectors. */

let default_growth_rate: float;

/** Constructs a vector with the specified growth rate and capacity. */

let make:
  (~growth_rate: float=?, ~capacity: int=?, unit) => t('a, [ | `R | `W]);

/** Creates a copy of the specified vector. */

let copy: t('a, [> | `R]) => t('a, [ | `R | `W]);

/** Reinterprets the specified vector as a read-only vector. */

let as_read_only: t('a, [> | `R]) => t('a, [ | `R]);

/** Reinterprets the specified vector as a write-only vector. */

let as_write_only: t('a, [> | `W]) => t('a, [ | `W]);

/** Returns the length of the specified vector. */

let length: t('a, [> ]) => int;

/** Returns the capacity of the specified vector. */

let capacity: t('a, [> ]) => int;

/** Returns the growth rate of the specified vector. */

let growth_rate: t('a, [> | `R]) => float;

/** Sets the growth rate of the specified vector to the specified value. */

let set_growth_rate: (float, t('a, [ | `R | `W])) => unit;

/** Resets the vector to an empty state. */

let clear: t('a, [ | `R | `W]) => unit;

/** Gets the value in the vector at the specified index.
    @raise Invalid_argument if the index is out of bounds. */

let get_exn: (t('a, [> | `R]), int) => 'a;

/** Sets the value in the vector at the specified index to the specified value.
    @raise Invalid_argument if the index is out of bounds. */

let set_exn: (t('a, [> | `W]), int, 'a) => unit;

/** Gets the value in the vector at the specified index. Returns None if the index is out of range. */

let get: (t('a, [> | `R]), int) => option('a);

/** Sets the value in the vector at the specified index to the specified value. Returns false if the index is out of range. */

let set: (t('a, [> | `W]), int, 'a) => bool;

/** Increases the vector's capacity to be at least as large as the specified value. */

let ensure_capacity: (int, t('a, [> | `W])) => unit;

/** Increases the vector's capacity by the specified value. */

let reserve: (int, t('a, [> | `W])) => unit;

/** Shrinks the vector's internal buffer to only be as large as the vector's length. */

let shrink_to_fit: t('a, [ | `R | `W]) => unit;

/** Pushes the specified item onto the end of the vector. */

let push: ('a, t('a, [> | `W])) => unit;

/** Pops off the item from the end of the vector. */

let pop: t('a, [ | `R | `W]) => option('a);

/** Maps the specified function over the vector, returning a new vector. (Functor [map] operation) */

let map: ('a => 'b, t('a, [> | `R])) => t('b, [ | `R | `W]);

/** Like {!map}, but the function also takes the item's index as a parameter. */

let mapi: ((int, 'a) => 'b, t('a, [> | `R])) => t('b, [ | `R | `W]);

/** Like {!map}, but the transformation is done in-place. */

let map_in_place: ('a => 'a, t('a, [ | `R | `W])) => unit;

/** Returns a singleton vector containing the specified item. (Applicative functor [pure] operation) */

let singleton: 'a => t('a, [ | `R | `W]);

/** Maps the specified function over all combinations of tuples from the 2 specified vectors, returning a new vector. (Applicative functor [liftA2] operation */

let map2:
  (('a, 'b) => 'c, t('a, [> | `R]), t('b, [> | `R])) => t('c, [ | `R | `W]);

/** Applies every function from the first vector to every value from the second vector, returning a new vector. (Applicatve functor [ap] operation) */

let apply: (t('a => 'b, [> | `R]), t('a, [> | `R])) => t('b, [ | `R | `W]);

/** Flattens nested vectors into a single, one-dimensional vector. (Monad [join] operation) */

let flatten: t(t('a, [> | `R]), [> | `R]) => t('a, [ | `R | `W]);

/** Like {!map}, but flattens the result. (Monad [bind] operation) */

let flat_map:
  ('a => t('b, [> | `R]), t('a, [> | `R])) => t('b, [ | `R | `W]);

/** Cartesian product of 2 vectors. (Equivalent to [liftA2 (,)]) */

let cartesian_product:
  (t('a, [> | `R]), t('b, [> | `R])) => t(('a, 'b), [ | `R | `W]);

/** Applies the specified function to each item in the vector. */

let iter: ('a => unit, t('a, [> | `R])) => unit;

/** Like {!iter}, but the function also takes the item's index as a parameter. */

let iteri: ((int, 'a) => unit, t('a, [> | `R])) => unit;

/** Returns a new vector containing only the items from the first vector that satisfy the specified predicate. */

let filter: ('a => bool, t('a, [> | `R])) => t('a, [ | `R | `W]);

/** Like {!filter}, but the predicate also takes the item's index as a parameter. */

let filteri: ((int, 'a) => bool, t('a, [> | `R])) => t('a, [ | `R | `W]);

/** Performs a filter in-place, based on the specified predicate. */

let filter_in_place: ('a => bool, t('a, [ | `R | `W])) => unit;

/** Constructs a vector from the specified list. */

let of_list: list('a) => t('a, [ | `R | `W]);

/** Constructs a list from the specified vector. */

let to_list: t('a, [> | `R]) => list('a);

/** Constructs a vector from the specified array. */

let of_array: array('a) => t('a, [ | `R | `W]);

/** Constructs an array from the specified vector. */

let to_array: t('a, [> | `R]) => array('a);

/** Returns a new vector that contains all the items in the specified vector, but in reverse order. */

let rev: t('a, [> | `R]) => t('a, [ | `R | `W]);

/** Reverses the vector in-place. */

let rev_in_place: t('a, [ | `R | `W]) => unit;

/** Appends the second vector to the first vector. */

let append: (t('a, [ | `R | `W]), t('a, [> | `R])) => unit;

/** Returns [true] if any item in the vector satisfies the specified predicate. */

let exists: ('a => bool, t('a, [> | `R])) => bool;

/** Returns [true] if all items in the vector satisfies the specified predicate. */

let for_all: ('a => bool, t('a, [> | `R])) => bool;

/** Returns [true] if the specified item exists in the vector. Uses structural equality. */

let mem: ('a, t('a, [> | `R])) => bool;

/** Returns [true] if the specified item exists in the vector. Uses physical equality. */

let memq: ('a, t('a, [> | `R])) => bool;

/** Folds the specified function and default value over the array, from left to right. */

let fold_left: (('b, 'a) => 'b, 'b, t('a, [> | `R])) => 'b;

/** Folds the specified function and default value over the array, from right to left. */

let fold_right: (('a, 'b) => 'b, t('a, [> | `R]), 'b) => 'b;

/** Zips the two vectors together. */

let zip: (t('a, [> | `R]), t('b, [> | `R])) => t(('a, 'b), [ | `R | `W]);

/** Zips the two vectors together, using the specified function to combine values. */

let zip_with:
  (('a, 'b) => 'c, t('a, [> | `R]), t('b, [> | `R])) => t('c, [ | `R | `W]);

/** Sorts the specified vector. */

let sort: t('a, [ | `R | `W]) => unit;

/** Sorts the specified vector using the specified comparison function. */

let sort_by: (('a, 'a) => int, t('a, [ | `R | `W])) => unit;

/** Compares two vectors for equality. */

let equal: (t('a, [> | `R]), t('a, [> | `R])) => bool;

/** Compares two vectors for equality, using the specified equality function for elements. */

let equal_by: (('a, 'a) => bool, t('a, [> | `R]), t('a, [> | `R])) => bool;

/** Compares two vectors lexicographically. */

let compare: (t('a, [> | `R]), t('a, [> | `R])) => int;

/** Compares two vectors lexicographically, using the specified comparison function for elements. */

let compare_by: (('a, 'a) => int, t('a, [> | `R]), t('a, [> | `R])) => int;

/** Returns a string representation of the vector, using the specified function to format each value. */

let pretty_print: ('a => string, t('a, [> | `R])) => string;

/** Constructs a vector containing all numbers in the specified range. */

let iota: (int, int) => t(int, [ | `R | `W]);

/** Contains infix versions of some vector operations. */

module Infix: {
  /** Infix version of {!get_exn}. */
  // let (let.get_exn) = get_exn;
  // let (let.set_exn) = set_exn;

  // let (let.get) = get;
  // let (let.set) = set;
  let (let.get_exn): (t('a, [> | `R]), int) => 'a;

  /** Infix version of {!set_exn}. */

  let (let.set_exn): (t('a, [> | `W]), int, 'a) => unit;

  /** Infix version of {!get}. */

  let (let.get): (t('a, [> | `R]), int) => option('a);

  /** Infix version of {!set}. */

  let (let.set): (t('a, [> | `W]), int, 'a) => bool;

  /** Infix version of {!map}. */

  let (=|<): ('a => 'b, t('a, [> | `R])) => t('b, [ | `R | `W]);

  /** Infix version of {!map}, with the arguments flipped. */

  let (>|=): (t('a, [> | `R]), 'a => 'b) => t('b, [ | `R | `W]);

  /** Infix version of {!map}. */

  let (<$>): ('a => 'b, t('a, [> | `R])) => t('b, [ | `R | `W]);

  /** Infix version of {!apply}. */

  let (<*>):
    (t('a => 'b, [> | `R]), t('a, [> | `R])) => t('b, [ | `R | `W]);

  /** Infix version of {!flat_map}. */

  let (=<<):
    ('a => t('b, [> | `R]), t('a, [> | `R])) => t('b, [ | `R | `W]);

  /** Infix version of {!flat_map}, with the arguments flipped. */

  let (>>=):
    (t('a, [> | `R]), 'a => t('b, [> | `R])) => t('b, [ | `R | `W]);

  /** Infix version of {!iota}. */

  let (--): (int, int) => t(int, [ | `R | `W]);
};

/** Provides support for OCaml 4.08's binding operator syntax. */

module Let_syntax: {
  /** Equivalent to {!map}, but with the arguments flipped. */

  let (let+): (t('a, [> | `R]), 'a => 'b) => t('b, [ | `R | `W]);

  /** Equivalent to {!cartesian_product}. */

  let (and+):
    (t('a, [> | `R]), t('b, [> | `R])) => t(('a, 'b), [ | `R | `W]);

  /** Equivalent to {!flat_map}, but with the arguments flipped. */

  let ( let* ):
    (t('a, [> | `R]), 'a => t('b, [> | `R])) => t('b, [ | `R | `W]);

  /** Equivalent to {!cartesian_product}. */

  let ( and* ):
    (t('a, [> | `R]), t('b, [> | `R])) => t(('a, 'b), [ | `R | `W]);
};
