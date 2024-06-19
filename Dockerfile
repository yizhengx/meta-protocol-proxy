FROM ghcr.io/aeraki-mesh/meta-protocol-proxy:1.4.2
COPY bazel-bin/envoy /usr/local/bin/envoy