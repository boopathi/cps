cps
===

Copy as symlinks. Re-create Directory tree and symlink Files with customizations in renaming the tree elements
`
boopathi cps(master)> tree test
test
|-- four
|-- one
|   |-- dir1
|   |   |-- f1
|   |   |-- f2
|   |-- dir2
|   |-- f1
|   |-- f2
|-- three
|   |-- f1
|   |-- f2
|-- two
    |-- dir1
        |-- dir2
            |-- f1
                |-- f2
8 directories, 8 files
`

`
boopathi cps(master)> ./cps -v -s 'f/e' -s 'a/b' -s 'on/two' test b
DIR: test -> b
DIR: test/three -> b/three
LNK: test/three/f2 -> b/three/e2
LNK: test/three/f1 -> b/three/e1
DIR: test/one -> b/twoe
LNK: test/one/f2 -> b/twoe/e2
DIR: test/one/dir2 -> b/twoe/dir2
LNK: test/one/f1 -> b/twoe/e1
DIR: test/one/dir1 -> b/twoe/dir1
LNK: test/one/dir1/f2 -> b/twoe/dir1/e2
LNK: test/one/dir1/f1 -> b/twoe/dir1/e1
DIR: test/four -> b/eour
DIR: test/two -> b/two
LNK: test/two/f2 -> b/two/e2
DIR: test/two/dir2 -> b/two/dir2
LNK: test/two/f1 -> b/two/e1
DIR: test/two/dir1 -> b/two/dir1
`

`
boopathi cps(master)> tree b
b
|-- eour
|-- three
|   |-- e1 -> test/three/f1
|   |-- e2 -> test/three/f2
|-- two
|   |-- dir1
|   |-- dir2
|   |-- e1 -> test/two/f1
|   |-- e2 -> test/two/f2
|-- twoe
    |-- dir1
        |   |-- e1 -> test/one/dir1/f1
            |   |-- e2 -> test/one/dir1/f2
                |-- dir2
                    |-- e1 -> test/one/f1
                        |-- e2 -> test/one/f2
                        8 directories, 8 files
`
