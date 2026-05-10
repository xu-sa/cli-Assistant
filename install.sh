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
VERSION="1.0.8"
BINARY_NAME="syagent"
BINARY_NAME_="schat"
AGENT_NAME="linux-bot"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BINARY_A="$SCRIPT_DIR/build/bin/$BINARY_NAME-$VERSION"
BINARY_B="$SCRIPT_DIR/build/bin/$BINARY_NAME_-$VERSION"
PORT=9995
check_binary() {
    echo -e "${YELLOW}checking file...${NC}"
    
    if [ ! -f "$BINARY_A" ]; then
        echo -e "${RED}ERROR: not found $BINARY_A${NC}"
        exit 1
    fi
    if [ ! -f "$BINARY_B" ]; then
        echo -e "${RED}ERROR: not found $BINARY_B${NC}"
        exit 1
    fi
    
    if [ ! -x "$BINARY_A" ]; then
        chmod +x "$BINARY_A"
    fi
    
    if [ ! -x "$BINARY_B" ]; then
        chmod +x "$BINARY_B"
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
    
    touch "$INSTALL_DIR/$BINAR_NAME"
    cp "$BINARY_A" "$INSTALL_DIR/$BINARY_NAME"
    chmod +x "$INSTALL_DIR/$BINARY_NAME"

    touch "$HOME/.local/bin/$BINARY_NAME_"
    cp "$BINARY_B" "$HOME/.local/bin/$BINARY_NAME_"
    chmod +x "$HOME/.local/bin/$BINARY_NAME_"
    
    echo -e "${GREEN}✓ copied binary${NC}"
}
create_service_file() {
    echo -e "${YELLOW}创建systemd服务文件...${NC}"
    
    cat > "$SYSTEMD_USER_DIR/$SERVICE_NAME.service" << EOF
[Unit]
Description = a efficient terminal assistant

[Service]
Type=simple
User=$USER
WorkingDirectory=$INSTALL_DIR
ExecStart=$INSTALL_DIR/$BINARY_NAME $HOME/ $AGENT_NAME $PORT
# Environment="CONFIG_DIR=$CONFIG_DIR"
# Environment="LOG_DIR=$LOG_DIR"

Restart=no
StartLimitInterval=60
StartLimitBurst=5

StandardOutput=append:$LOG_DIR/.syagent.log
StandardError=append:$LOG_DIR/.syagent-error.log
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
    mkdir -p "$HOME/.syagent"
    mkdir -p "$HOME/.syagent/$AGENT_NAME"
    cp ./assets/RULE.md "$HOME/.syagent/"
    cp ./assets/IDENTITY.md "$HOME/.syagent/$AGENT_NAME/"
    cp ./assets/SUBRULE.md "$HOME/.syagent/$AGENT_NAME/"
    
    EXTENSION_DIR="$HOME/.syagent/.extension"
    echo "Workspace created at: $HOME/.syagent"
    
    mkdir -p "$EXTENSION_DIR"
    python3 -m venv "$EXTENSION_DIR/venv"
    "$EXTENSION_DIR/venv/bin/pip" install requests
    source "$EXTENSION_DIR/venv/bin/activate"
    pip install --upgrade pip
    deactivate
    
    mkdir -p "$EXTENSION_DIR/help"
    cp "./assets/help/help.py" "$EXTENSION_DIR/help/"
    cp "./assets/help/help.json" "$EXTENSION_DIR/help/"
    
    mkdir -p "$EXTENSION_DIR/pinchtab_peek"
    cp "./assets/pinchtab_peek/pinchtab_peek.py" "$EXTENSION_DIR/pinchtab_peek/"
    cp "./assets/pinchtab_peek/pinchtab_peek.json" "$EXTENSION_DIR/pinchtab_peek/"

    mkdir -p "$EXTENSION_DIR/pinchtab_do"
    cp "./assets/pinchtab_do/pinchtab_do.py" "$EXTENSION_DIR/pinchtab_do/"
    cp "./assets/pinchtab_do/pinchtab_do.json" "$EXTENSION_DIR/pinchtab_do/"
    
    echo "Extension environment created at: $EXTENSION_DIR"
}
main() {
    echo -e "${GREEN}start to install Syagent user service...${NC}"
    setup_workspace
    check_binary
    create_directories
    copy_files
    create_service_file
    echo "Service start On port $PORT"
    setup_systemd
    start_service
}   
main
