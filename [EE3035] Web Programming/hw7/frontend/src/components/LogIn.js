import { UserOutlined } from "@ant-design/icons";
import { Input } from "antd";

const LogIn = ({me, setName, onLogin}) => {
    return (
        <Input.Search
            size="large"
            style={{ width: 300, margin: 50 }}
            prefix={<UserOutlined />}
            placeholder="Enter your name"
            value={me}
            onChange={(e) => setName(e.target.value)}
            enterButton="Sign In"
            onSearch={(name) => onLogin(name)}
        />
    );
}

export default LogIn;