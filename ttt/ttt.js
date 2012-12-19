var STR_TRUE = 'T';
var STR_FALSE = 'F';

var vars = [];
var ops = [];

var lol = [
  // tt, tf, ft, ff, friendly-name
  [STR_FALSE,STR_FALSE,STR_FALSE,STR_FALSE,'false'],
  [STR_FALSE,STR_FALSE,STR_FALSE,STR_TRUE,'nor'],
  [STR_FALSE,STR_FALSE,STR_TRUE,STR_FALSE,''],
  [STR_FALSE,STR_FALSE,STR_TRUE,STR_TRUE,'not A'],
  [STR_FALSE,STR_TRUE,STR_FALSE,STR_FALSE,''],
  [STR_FALSE,STR_TRUE,STR_FALSE,STR_TRUE,'not B'],
  [STR_FALSE,STR_TRUE,STR_TRUE,STR_FALSE,'xor'],
  [STR_FALSE,STR_TRUE,STR_TRUE,STR_TRUE,'nand'],
  [STR_TRUE,STR_FALSE,STR_FALSE,STR_FALSE,'and'],
  [STR_TRUE,STR_FALSE,STR_FALSE,STR_TRUE,'iff'],
  [STR_TRUE,STR_FALSE,STR_TRUE,STR_FALSE,'B'],
  [STR_TRUE,STR_FALSE,STR_TRUE,STR_TRUE,'->'],
  [STR_TRUE,STR_TRUE,STR_FALSE,STR_FALSE,'A'],
  [STR_TRUE,STR_TRUE,STR_FALSE,STR_TRUE,''],
  [STR_TRUE,STR_TRUE,STR_TRUE,STR_FALSE,'or'],
  [STR_TRUE,STR_TRUE,STR_TRUE,STR_TRUE,'true'],
];

function get_binary_operator_name(bb, i)
{
  if ( bb[i][4] != '' )
    return bb[i][4];
  else
    return '0x' + i.toString(16);
}

function generate_bin_bool()
{
  return lol;
}

function create_header_row(vars, bb)
{
  var out = document.createElement('tr');

  for ( var i = 0; i < vars; ++ i )
  {
    var td = document.createElement('th');
    td.className = "op_tbl_var";
    td.innerHTML = String.fromCharCode('A'.charCodeAt(0)+i);
    out.appendChild(td);
  }
  
  for ( var i = 0; i < bb.length; ++ i )
  {
    var td = document.createElement('th');
    td.className = "op_tbl_op";
    td.innerHTML = get_binary_operator_name(bb, i);
    out.appendChild(td);
  }

  var thead = document.createElement('thead');
  thead.appendChild(out);

  return thead;
}

function initialize_operators()
{
  var bbt = $('#binboolop');

  var bb = generate_bin_bool();

  var opdrop = $('[name=op]').empty();
  for ( var i = 0; i < bb.length; ++ i )
  {
    var op = document.createElement('option');
    op.setAttribute('value', i);
    op.innerHTML = get_binary_operator_name(bb,i);
    opdrop.append(op);
  }

  // header row
  var h = create_header_row(2, bb);
  bbt.empty().append(h);

  var ab = [[STR_TRUE,STR_TRUE], [STR_TRUE,STR_FALSE], [STR_FALSE,STR_TRUE], [STR_FALSE,STR_FALSE]];
  for ( var i = 0; i < ab.length; ++ i )
  {
    var row = document.createElement('tr');

    for ( var j = 0; j < ab[i].length; ++ j )
    {
      var cell = document.createElement('td');
      cell.innerHTML = ab[i][j];
      row.appendChild(cell);
    }

    for ( var j = 0; j < bb.length; ++ j )
    {
      var cell = document.createElement('td');
      cell.innerHTML = bb[j][i];
      row.appendChild(cell);
    }

    bbt.append(row);
  }
}

function add_droperand(txt, idx)
{
  var op = document.createElement('option');
  op.setAttribute('value', idx);
  op.innerHTML = txt;
  $('[name=lop]').append(op);
  $('[name=rop]').append($(op).clone());
}

function get_operation_result ( bb, l, r, op )
{
  if ( l == STR_TRUE )
  {
    if ( r == STR_TRUE )
      return bb[op][0];
    else
      return bb[op][1];
  }
  else
  {
    if ( r == STR_TRUE )
      return bb[op][2];
    else
      return bb[op][3];
  }
}

function add_calculated_column_to_table(lop, op, rop, name)
{
  if ( lop && op && rop && name )
  {
    var thead_tr = $('#tt > thead > tr');
    var bb = generate_bin_bool();
    var th = document.createElement('th');
    th.className = 'op_tbl_op';
    th.innerHTML = name;
    thead_tr.append(th);

    var datarows = $('#tt > tbody > tr')

    for ( var i = 0; i < datarows.length; ++ i )
    {
      var td = document.createElement('td');
      var l = $(datarows[i]).children('td')[lop-1].innerHTML;
      var r = $(datarows[i]).children('td')[rop-1].innerHTML;
      td.innerHTML = get_operation_result(bb, l, r, op);
      td.className = 'op_tbl_op';
      $(datarows[i]).append(td);
    }

    add_droperand(name, thead_tr.children('th').last().index()+1);
  }
}

function add_variable_to_table()
{
  var v = document.createElement('th');
  v.className = 'op_tbl_var';

  var t = $('#tt');
  var thead_tr = t.find('thead > tr');
  v.innerHTML = String.fromCharCode('a'.charCodeAt(0)+thead_tr.children('.op_tbl_var').length);
  thead_tr.append(v);

  var tbody = t.find('tbody');
  var datarows = tbody.children();

  if ( datarows.length > 0 )
  {
    for ( var i = 0; i < datarows.length; ++ i )
    {
      var clonus = $(datarows[i]).clone();

      var td = document.createElement('td');
      td.innerHTML = STR_TRUE;
      td.className = 'varcell';
      // $(datarows[i]).children('.varcell').last().after(td);
      $(datarows[i]).append(td);

      var td = document.createElement('td');
      td.innerHTML = STR_FALSE;
      td.className = 'varcell';
      // $(clonus).children('.varcell').last().after(td);
      $(clonus).append(td);

      tbody.append(clonus);
    }
  }
  else
  {
    var tr = document.createElement('tr');
    tr.className = 'datarow';
    var td = document.createElement('td');
    td.className = 'varcell';
    td.innerHTML = STR_TRUE;
    tr.appendChild(td);
    tbody.append(tr);

    var tr = document.createElement('tr');
    tr.className = 'datarow';
    var td = document.createElement('td');
    td.className = 'varcell';
    td.innerHTML = STR_FALSE;
    tr.appendChild(td);
    tbody.append(tr);
  }

  add_droperand(v.innerHTML, thead_tr.children('th').last().index()+1);
}

function addvariable()
{
  add_variable_to_table();
}

function addop()
{
  add_calculated_column_to_table($('[name=lop]').val(), $('[name=op]').val(), $('[name=rop]').val(), $('[name=name]').val());
}

$(window).load(function() {
    initialize_operators();
});

