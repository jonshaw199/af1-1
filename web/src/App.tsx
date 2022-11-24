import { useContext, useEffect, useRef, useState } from "react";
import { styled, ThemeProvider } from "@mui/material/styles";

import "./App.css";
import Nav from "./components/Nav";
import Dashboard from "./components/Dashboard";
import Messages from "./components/Messages";
import theme from "./theme";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import Users from "./components/Users";
import Settings from "./components/Settings";
import { GlobalUserContext, useUserState } from "./state/user";
import { useMemo } from "react";
import Login from "./components/Login";
import { useAF1Websocket } from "./hooks/useWebsocket";
import Orgs from "./components/Orgs";
import { GlobalOrgContext, useOrgState } from "./state/org";
import { Box } from "@mui/system";
import { CircularProgress } from "@mui/material";
import Devices from "./components/Devices";
import { GlobalDeviceContext, useDeviceState } from "./state/device";
import { GlobalMessageContext, useMessageState } from "./state/message";
import Lights from "./components/Lights";

const Main = styled("main", { shouldForwardProp: (prop) => prop !== "open" })<{
  open?: boolean;
}>(({ theme, open }) => ({
  padding: theme.spacing(1),
  transition: theme.transitions.create("margin", {
    easing: theme.transitions.easing.sharp,
    duration: theme.transitions.duration.leavingScreen,
  }),
  marginLeft: 0,
  ...(open && {
    transition: theme.transitions.create("margin", {
      easing: theme.transitions.easing.easeOut,
      duration: theme.transitions.duration.enteringScreen,
    }),
    marginLeft: theme.drawer.width,
  }),
}));

const Outer = styled("div")(({ theme }) => ({
  height: "100vh",
  backgroundColor: theme.page.backgroundColor,
}));

function LoggedIn() {
  const [open, setOpen] = useState(false);
  const { currentUser } = useContext(GlobalUserContext);

  useAF1Websocket({
    url: `ws://127.0.0.1:3000/lights/ws?deviceId=${process.env.REACT_APP_DEVICE_ID}`,
    onRecv: (m) => console.log(m),
  });

  return (
    <>
      <Nav
        open={open}
        onOpen={() => setOpen(true)}
        onClose={() => setOpen(false)}
      />
      <Main open={open}>
        <Routes>
          <Route path="/" element={<Dashboard />} />
          <Route path="messages/" element={<Messages />} />
          <Route path="users/" element={<Users />} />
          <Route path="settings/" element={<Settings />} />
          <Route path="orgs/" element={<Orgs />} />
          <Route path="devices/" element={<Devices />} />
          <Route path="lights/" element={<Lights />} />
        </Routes>
      </Main>
    </>
  );
}

function App() {
  const userState = useUserState();
  const { token, currentUser, authWithToken, getList: getUserList } = userState;
  const loggedIn = useMemo(() => token && currentUser, [token, currentUser]);
  const orgState = useOrgState();
  const { getList: getOrgList } = orgState;
  const initialLoadRef = useRef(true);
  const [loadingInitially, setLoadingInitially] = useState(true);
  const deviceState = useDeviceState();
  const { getList: getDeviceList } = deviceState;
  const messageState = useMessageState();
  const { getList: getMessageList } = messageState;

  useEffect(() => {
    if (initialLoadRef.current) {
      initialLoadRef.current = false;
      setTimeout(() => setLoadingInitially(false), 1000);
      authWithToken();
    }
  }, [authWithToken]);

  useEffect(() => {
    if (token) {
      getOrgList();
      getUserList();
      getDeviceList();
      getMessageList();
    }
  }, [token, getOrgList, getUserList, getDeviceList, getMessageList]);

  return (
    <Router>
      <ThemeProvider theme={theme}>
        <GlobalOrgContext.Provider value={orgState}>
          <GlobalUserContext.Provider value={userState}>
            <GlobalDeviceContext.Provider value={deviceState}>
              <GlobalMessageContext.Provider value={messageState}>
                <Outer>
                  {loadingInitially ? (
                    <Box
                      display="flex"
                      justifyContent="center"
                      alignItems="center"
                      height={1}
                    >
                      <CircularProgress />
                    </Box>
                  ) : loggedIn ? (
                    <LoggedIn />
                  ) : (
                    <Login />
                  )}
                </Outer>
              </GlobalMessageContext.Provider>
            </GlobalDeviceContext.Provider>
          </GlobalUserContext.Provider>
        </GlobalOrgContext.Provider>
      </ThemeProvider>
    </Router>
  );
}

export default App;
