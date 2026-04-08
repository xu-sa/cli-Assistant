#!/bin/bash
set -e
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'
SERVICE_NAME="syagent"
INSTALL_DIR="$HOME/.local/share/syagent"
CONFIG_DIR=$INSTALL_DIR
LOG_DIR="$INSTALL_DIR/logs"
SYSTEMD_USER_DIR="$HOME/.config/systemd/user"
BINARY_NAME="syagent"
AGENT_NAME="linux-bot"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BINARY_PATH="$SCRIPT_DIR/test/bin/$BINARY_NAME"
check_binary() {
    echo -e "${YELLOW}checking file...${NC}"
    
    if [ ! -f "$BINARY_PATH" ]; then
        echo -e "${RED}ERROR: not found $BINARY_PATH${NC}"
        exit 1
    fi
    
    if [ ! -x "$BINARY_PATH" ]; then
        chmod +x "$BINARY_PATH"
    fi
    
    echo -e "${GREEN}✓ Binary checking passed ${NC}"
}
create_directories() {
    echo -e "${YELLOW}creating cache space...${NC}"
    
    mkdir -p "$INSTALL_DIR"
    mkdir -p "$LOG_DIR"
    mkdir -p "$CONFIG_DIR"
    mkdir -p "$SYSTEMD_USER_DIR"
    
    echo -e "${GREEN}✓ Directory created ${NC}"
}
copy_files() {
    echo -e "${YELLOW}copying...${NC}"

    cp "$BINARY_PATH" "$INSTALL_DIR/"
    chmod +x "$INSTALL_DIR/$BINARY_NAME"
    
    echo -e "${GREEN}✓ copied binary${NC}"
}
create_service_file() {
    echo -e "${YELLOW}创建systemd服务文件...${NC}"
    
    cat > "$SYSTEMD_USER_DIR/$SERVICE_NAME.service" << EOF
[Unit]
Description=~~the most efficient terminal assistant~~

[Service]
Type=simple
User=$USER
WorkingDirectory=$INSTALL_DIR
ExecStart=$INSTALL_DIR/$BINARY_NAME $HOME/ $AGENT_NAME 9995
# Environment="CONFIG_DIR=$CONFIG_DIR"
# Environment="LOG_DIR=$LOG_DIR"

Restart=no
StartLimitInterval=60
StartLimitBurst=5

StandardOutput=append:$LOG_DIR/syagent.log
StandardError=append:$LOG_DIR/syagent-error.log
SyslogIdentifier=$SERVICE_NAME
 
MemoryLimit=100M
CPUQuota=30%
LimitNOFILE=65536
 
NoNewPrivileges=false
PrivateTmp=false
PrivateDevices=false
ProtectHome=no
ProtectSystem=strict
ReadWritePaths=$LOG_DIR $CONFIG_DIR
# Environment="HOME=$HOME"
# Environment="USER=$USER"

[Install]
WantedBy=default.target
EOF
    
    echo -e "${GREEN}✓ 服务文件创建完成${NC}"
}

setup_systemd() {
    echo -e "${YELLOW}配置用户systemd...${NC}"
    systemctl --user daemon-reload
    echo -e "${GREEN}✓ systemd配置完成${NC}"
}

start_service() {
    echo -e "${YELLOW}activating SYagent service...${NC}"
    systemctl --user enable $SERVICE_NAME
    systemctl --user start $SERVICE_NAME
    sleep 2
    if systemctl --user is-active --quiet $SERVICE_NAME; then
        echo -e "${GREEN}✓ Syagent service started${NC}"
    else
        echo -e "${RED}✗ Syagent service cant initialize${NC}"
        systemctl --user status $SERVICE_NAME --no-pager
        exit 1
    fi
}
setup_workspace(){
    mkdir -p "$HOME/.slcache"
    cp ./assets/RULE.md "$HOME/.slcache/"
    mkdir -p "$HOME/.slcache/$AGENT_NAME"
    cp ./assets/IDENTITY.md "$HOME/.slcache/$AGENT_NAME/"
    cp ./assets/SUBRULE.md "$HOME/.slcache/$AGENT_NAME/"
}
main() {
    echo -e "${GREEN}start to install Syagent user service...${NC}"
    echo ""
    setup_workspace
    check_binary
    create_directories
    copy_files
    create_service_file
    setup_systemd
    start_service
}   
main
