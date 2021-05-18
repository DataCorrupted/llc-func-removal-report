docker build . --tag codegen-bug
docker run codegen-bug bash -c "cd /codegen-bug/src && make test"