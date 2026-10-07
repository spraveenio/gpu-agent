#!/usr/bin/env bash
set -x
set -euo pipefail
dir=/usr/src/github.com/ROCm/gpu-agent
netns=/var/run/netns

PATH=/opt/gpuagent-deps/bin:/usr/local/go/bin:$PATH

trap term INT TERM

mkdir -p ${dir}
mkdir -p ${netns}
rm -f $dir/.container_ready
git config --global --add safe.directory $dir || true
export GOFLAGS=-mod=vendor

if [[ -n "${USER_NAME:-}" && -n "${USER_UID:-}" && -n "${USER_GID:-}" ]]; then
	echo "Creating user ${USER_NAME} with UID=${USER_UID}, GID=${USER_GID}..."

	if ! getent group "$USER_GID" >/dev/null; then
		groupadd -g "$USER_GID" "$USER_NAME"
	fi

	if ! id -u "$USER_NAME" >/dev/null 2>&1; then
		useradd -m -u "$USER_UID" -g "$USER_GID" -s /bin/bash "$USER_NAME"
		# Add user to wheel group for sudo on RHEL
		if ! grep -qi "ubuntu" /etc/os-release; then
			usermod -aG wheel "$USER_NAME"
		fi
		echo "$USER_NAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USER_NAME
		chmod 0440 /etc/sudoers.d/$USER_NAME
	fi

	chown -R "$USER_UID":"$USER_GID" $dir
	chown -R "$USER_UID":"$USER_GID" /home/$USER_NAME

	su - "$USER_NAME" -c "echo 'export GOPATH=/home/$USER_NAME/go' >> ~/.bashrc"
	su - "$USER_NAME" -c "echo 'export PATH=\$GOPATH/bin:/usr/local/go/bin:/opt/gpuagent-deps/bin:\$PATH' >> ~/.bashrc"
	su - "$USER_NAME" -c "echo 'export PATH=/usr/local/go/bin:/opt/gpuagent-deps/bin:\$PATH' >> ~/.bashrc"
	su - "$USER_NAME" -c "echo 'export GPUAGENT_DEPS_PREFIX=/opt/gpuagent-deps' >> ~/.bashrc"
	su - "$USER_NAME" -c "echo 'export LD_LIBRARY_PATH=/opt/gpuagent-deps/lib:/opt/gpuagent-deps/lib64:\${LD_LIBRARY_PATH:-}' >> ~/.bashrc"
	exec su - "$USER_NAME" -c "$@"
else
	echo "Running as default user (root)..."
    touch $dir/.container_ready
    exec "$@"
fi
touch $dir/.container_ready
exec "$@"
