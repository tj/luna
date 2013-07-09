
for file in test/parser/*.luna; do
  out=${file/luna/out}
  ast=$(./luna --ast $file)
  expected=$(cat $out)
  if [[ "$ast" != "$expected" ]]; then
    echo
    echo "  $file failed:"
    echo
    echo "  expected:"
    echo "  '$expected''"
    echo
    echo "  got:"
    echo "  '$ast'"
    echo
    exit 1
  fi
done
