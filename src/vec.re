type t('a, -'p) = {
  mutable growth_rate: float,
  mutable length: int,
  mutable data: array('a),
};

let default_growth_rate = 2.;

[@inline]
let array_uninit = n => Array.make(n, Obj.magic(0));

let make = (~growth_rate as gr=default_growth_rate, ~capacity as c=0, ()) =>
  if (gr <= 1.) {
    raise(Invalid_argument("growth_rate <= 1"));
  } else if (c < 0) {
    raise(Invalid_argument("capacity < 0"));
  } else {
    {growth_rate: gr, length: 0, data: array_uninit(c)};
  };

external as_read_only: t('a, [> | `R]) => t('a, [ | `R]) = "%identity";

external as_write_only: t('a, [> | `W]) => t('a, [ | `W]) = "%identity";

[@inline]
let length = v => v.length;
[@inline]
let capacity = v => Array.length(v.data);

[@inline]
let growth_rate = v => v.growth_rate;
let set_growth_rate = (gr, v) =>
  if (gr <= 1.) {
    raise(Invalid_argument("growth_rate <= 1"));
  } else {
    v.growth_rate = gr;
  };

[@inline]
let clear = v => {
  v.length = 0;
  v.data = [||];
};

[@inline]
let get_exn = (v, i) =>
  if (i < 0 || i >= v.length) {
    raise(Invalid_argument("Index out of range"));
  } else {
    v.data[i];
  };

[@inline]
let set_exn = (v, i, a) =>
  if (i < 0 || i >= v.length) {
    raise(Invalid_argument("Index out of range"));
  } else {
    v.data[i] = a;
  };

let get = (v, i) =>
  if (i < 0 || i >= v.length) {
    None;
  } else {
    Some(v.data[i]);
  };

[@inline]
let set = (v, i, a) =>
  i >= 0
  && i < v.length
  && {
    v.data[i] = a;
    true;
  };

let ensure_capacity = (c, v) => {
  let capacity = capacity(v);
  if (c < 0) {
    raise(Invalid_argument("capacity < 0"));
  } else if (c > capacity) {
    let cap =
      ref(
        if (capacity == 0) {
          v.growth_rate;
        } else {
          float_of_int(capacity);
        },
      );
    let c = float_of_int(c);
    while (cap^ < c) {
      cap := cap^ *. v.growth_rate;
    };

    let data = array_uninit(int_of_float(cap^));
    Array.blit(v.data, 0, data, 0, v.length);
    v.data = data;
  };
};

let reserve = (c, v) =>
  if (c < 0) {
    raise(Invalid_argument("amount_to_reserve < 0"));
  } else {
    ensure_capacity(capacity(v) + c, v);
  };

let shrink_to_fit = v =>
  if (capacity(v) > v.length) {
    let data = array_uninit(v.length);
    Array.blit(v.data, 0, data, 0, v.length);
    v.data = data;
  };

let push = (val', v) => {
  ensure_capacity(v.length + 1, v);
  let length = v.length;
  v.length = length + 1;
  v.data[length] = val';
};

let pop = v =>
  if (v.length == 0) {
    None;
  } else {
    let val' = v.data[v.length - 1];
    v.data[v.length - 1] = Obj.magic(0);
    v.length = v.length - 1;
    Some(val');
  };

[@inline]
let singleton = a => {
  growth_rate: default_growth_rate,
  length: 1,
  data: [|a|],
};

let map = (f, v) => {
  let v2 = make(~growth_rate=v.growth_rate, ~capacity=v.length, ());
  v2.length = v.length;

  for (i in 0 to v.length - 1) {
    v2.data[i] = f(v.data[i]);
  };

  v2;
};

let mapi = (f, v) => {
  let v2 = make(~growth_rate=v.growth_rate, ~capacity=v.length, ());
  v2.length = v.length;

  for (i in 0 to v.length - 1) {
    v2.data[i] = f(i, v.data[i]);
  };

  v2;
};

let map_in_place = (f, v) =>
  for (i in 0 to v.length - 1) {
    v.data[i] = f(v.data[i]);
  };

let map2 = (f, v1, v2) => {
  let total_l = v1.length * v2.length;
  let max_gr = max(v1.growth_rate, v2.growth_rate);

  let v = make(~growth_rate=max_gr, ~capacity=total_l, ());
  v.length = total_l;

  let idx = ref(0);

  for (i in 0 to v1.length - 1) {
    for (j in 0 to v2.length - 1) {
      v.data[idx^] = f(v1.data[i], v2.data[j]);
      incr(idx);
    };
  };

  v;
};

[@inline]
let apply = (f, v) => map2((@@), f, v);

let flatten = vs => {
  let max_gr = ref(0.);
  let total_l = ref(0);

  for (i in 0 to vs.length - 1) {
    let crr_v = vs.data[i];
    let v_gr = crr_v.growth_rate;
    if (max_gr^ < v_gr) {
      max_gr := v_gr;
    };

    total_l := total_l^ + crr_v.length;
  };

  let v = make(~growth_rate=max_gr^, ~capacity=total_l^, ());
  v.length = total_l^;

  let idx = ref(0);

  for (i in 0 to vs.length - 1) {
    let crr_v = vs.data[i];

    for (j in 0 to crr_v.length - 1) {
      v.data[idx^] = crr_v.data[j];
      incr(idx);
    };
  };

  v;
};

let append = (v, v2) => {
  let l = v.length + v2.length;
  ensure_capacity(l, v);

  for (i in 0 to v2.length - 1) {
    v.data[i + v.length] = v2.data[i];
  };

  v.length = l;
};

let flat_map = (f, v) => {
  let v2 = make(~growth_rate=v.growth_rate, ~capacity=v.length, ());

  for (i in 0 to v.length - 1) {
    append(v2, f(v.data[i]));
  };

  v2;
};

[@inline]
let cartesian_product = (a, b) => map2((a, b) => (a, b), a, b);

let iter = (f, v) =>
  for (i in 0 to v.length - 1) {
    f(v.data[i]);
  };

let iteri = (f, v) =>
  for (i in 0 to v.length - 1) {
    f(i, v.data[i]);
  };

let filter = (f, v) => {
  let v2 = make(~growth_rate=v.growth_rate, ~capacity=v.length, ());
  let l = ref(0);

  for (i in 0 to v.length - 1) {
    let e = v.data[i];
    if (f(e)) {
      v2.data[l^] = e;
      incr(l);
    };
  };

  v2.length = l^;
  v2;
};

let filteri = (f, v) => {
  let v2 = make(~growth_rate=v.growth_rate, ~capacity=v.length, ());
  let l = ref(0);

  for (i in 0 to v.length - 1) {
    let e = v.data[i];
    if (f(i, e)) {
      v2.data[l^] = e;
      incr(l);
    };
  };

  v2.length = l^;
  v2;
};

let filter_in_place = (f, v) => {
  let old_l = v.length;
  let l = ref(0);

  for (i in 0 to old_l - 1) {
    let e = v.data[i];
    if (f(e)) {
      v.data[l^] = e;
      incr(l);
    };
  };

  for (i in l^ to old_l - 1) {
    v.data[i] = Obj.magic(0);
  };

  v.length = l^;
};

[@inline]
let of_array_steal = a => {
  growth_rate: default_growth_rate,
  length: Array.length(a),
  data: a,
};

[@inline]
let of_array = a => of_array_steal(Array.copy(a));
[@inline]
let to_array = v => Array.sub(v.data, 0, v.length);

[@inline]
let of_list = l => of_array_steal(Array.of_list(l));

let to_list = v => {
  let rec go = acc =>
    fun
    | (-1) => acc
    | i => go([v.data[i], ...acc], i - 1);

  go([], v.length - 1);
};

[@inline]
let copy = v => of_array_steal(to_array(v));

let rev_in_place = v => {
  [@inline]
  let swap = (i, j) => {
    let temp = v.data[i];
    v.data[i] = v.data[j];
    v.data[j] = temp;
  };

  let rec go = (i, j) =>
    if (i < j) {
      swap(i, j);
      go(i + 1, j - 1);
    };

  go(0, v.length - 1);
};

[@inline]
let rev = v => {
  let v' = copy(v);
  rev_in_place(v');
  v';
};

let exists = (f, v) => {
  let rec go = i => i != v.length && (f(v.data[i]) || go(i + 1));
  go(0);
};

let for_all = (f, v) => {
  let rec go = i => i == v.length || f(v.data[i]) && go(i + 1);
  go(0);
};

[@inline]
let mem = e => exists((==)(e));
[@inline]
let memq = e => exists((===)(e));

let fold_left = (f, z, v) => {
  let rec go = (acc, i) =>
    if (i == v.length) {
      acc;
    } else {
      go(f(acc, v.data[i]), i + 1);
    };

  go(z, 0);
};

let fold_right = (f, v, z) => {
  let rec go = (acc, i) =>
    if (i <= 0) {
      acc;
    } else {
      go(f(v.data[i], acc), i - 1);
    };

  go(z, v.length - 1);
};

let zip_with = (f, v1, v2) => {
  let min_length = min(v1.length, v2.length);
  let max_gr = max(v1.growth_rate, v2.growth_rate);

  let v = make(~growth_rate=max_gr, ~capacity=min_length, ());
  v.length = min_length;

  for (i in 0 to min_length - 1) {
    v.data[i] = f(v1.data[i], v2.data[i]);
  };

  v;
};

[@inline]
let zip = (v1, v2) => zip_with((a, b) => (a, b), v1, v2);

[@inline]
let sort_by = (f, v) => {
  shrink_to_fit(v);
  Array.fast_sort(f, v.data);
};

[@inline]
let sort = v => sort_by(compare, v);

let equal_by = (f, a, b) =>
  if (a.length != b.length) {
    false;
  } else {
    let rec go = i => i == a.length || f(a.data[i], b.data[i]) && go(i + 1);
    go(0);
  };

[@inline]
let equal = (a, b) => equal_by((==), a, b);

let compare_by = (f, a, b) => {
  let (min_l, min_l_ord) =
    switch (a.length - b.length) {
    | 0 => (a.length, 0)
    | l when l < 0 => (a.length, (-1))
    | _ => (b.length, 1)
    };

  let rec go = i =>
    if (i == min_l) {
      min_l_ord;
    } else {
      let ord = f(a.data[i], b.data[i]);
      if (ord != 0) {
        ord;
      } else {
        go(i + 1);
      };
    };

  go(0);
};

[@inline]
let compare = (a, b) => compare_by(compare, a, b);

let pretty_print = (fmt, v) =>
  if (v.length == 0) {
    "[]";
  } else {
    let buf = Buffer.create(2);

    Buffer.add_char(buf, '[');
    Buffer.add_string(buf) @@ fmt(v.data[0]);

    for (i in 1 to v.length - 1) {
      Buffer.add_string(buf, "; ");
      Buffer.add_string(buf, fmt(v.data[i]));
    };

    Buffer.add_char(buf, ']');
    Buffer.contents(buf);
  };

let iota = (start, end') => {
  let l = abs(end' - start) + 1;
  let v = make(~capacity=l, ());
  let rec inc = (i, crr) =>
    if (crr <= end') {
      v.data[i] = crr;
      inc(i + 1, crr + 1);
    };

  let rec dec = (i, crr) =>
    if (crr >= end') {
      v.data[i] = crr;
      dec(i + 1, crr - 1);
    };

  if (start < end') {
    inc(0, start);
  } else {
    dec(0, start);
  };

  v.length = l;
  v;
};

module Infix = {
  let (let.get_exn) = get_exn;
  let (let.set_exn) = set_exn;

  let (let.get) = get;
  let (let.set) = set;

  let (=|<) = map;
  [@inline]
  let (>|=) = (v, f) => f =|< v;

  let (<$>) = map;
  let (<*>) = apply;

  let (=<<) = flat_map;
  let (>>=) = (v, f) => f =<< v;

  let (--) = iota;
};

module Let_syntax = {
  [@inline]
  let (let+) = (v, f) => map(f, v);
  let (and+) = cartesian_product;

  [@inline]
  let ( let* ) = (v, f) => flat_map(f, v);
  let ( and* ) = cartesian_product;
};
